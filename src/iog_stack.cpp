#include <stdlib.h>

#include "iog_assert.h"
#include "iog_stack.h"
#include "cli_colors.h"
#include "iog_memlib.h"

//--------------------- PUBLIC FUNCTIONS --------------------------------------------

/**
 * Allocates stack data with INIT_STACK_DATA_CAPACITY size and null values.
 * Turns isInitialized flag to 1.
 * @param[out] stack pointer to stack
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
IogStackReturnCode iog_stack_init(IogStack_t *stack) {
  IOG_CHECK_STACK_NULL( stack );

  if (stack->isInitialized) {
    return ERR_STACK_ALREADY_INITIALIZED;
  }

  stack->size = 0;
  
  IogStackReturnCode alloc_err = iog_stack_allocate_data(stack, INIT_STACK_DATA_CAPACITY);
  if (alloc_err != OK) {
    iog_stack_destroy(stack);
    return alloc_err;
  }
    
  iog_stack_update_canaries(stack);

  stack->isInitialized = 1;

  IOG_RETURN_IF_ERROR(iog_stack_verify(stack));

  return OK;
}

/**
 * Frees data and reset stack to zero
 * @param[out] stack pointer to stack
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
IogStackReturnCode iog_stack_destroy(IogStack_t *stack) {
  IOG_CHECK_STACK_NULL( stack );

  free(stack->firstDataCanary);

  stack->data = NULL;
  stack->firstDataCanary  = NULL;
  stack->secondDataCanary = NULL;

  stack->firstStackCanary  = 0;
  stack->secondStackCanary = 0;

  stack->size = 0;
  stack->capacity = 0;
  stack->isInitialized = 0;

  return OK;
}

/**
 * Adds value to the end of data array and increments size
 * @param[out] stack pointer to stack (can't be NULL)
 * @param[in]  value new stack value
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
IogStackReturnCode iog_stack_push (IogStack_t *stack, iog_stack_value_t value) {
  IOG_RETURN_IF_ERROR( iog_stack_verify(stack) );

  if (stack->size == stack->capacity) {
    IOG_RETURN_IF_ERROR( iog_stack_allocate_more(stack) );
  }

  stack->data[stack->size] = value;
  stack->size++;

  IOG_RETURN_IF_ERROR( iog_stack_verify(stack) );

  return OK;
}

/**
 * If size less 1/4 of capacity then frees rest memory.
 * @param[out] stack pointer to stack
 * @param[out] value pointer to variable in which want to write (can't be null)
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
IogStackReturnCode iog_stack_pop (IogStack_t *stack, iog_stack_value_t *value) {
  IOG_ASSERT(value);

  IOG_RETURN_IF_ERROR( iog_stack_verify(stack) );

  if (stack->size == 0)
    return ERR_STACK_UNDERFLOW;

  *value = stack->data[stack->size-1];
  stack->data[stack->size-1] = 0;
  stack->size--;

  if (stack->size <= stack->capacity / 4) {
    IOG_RETURN_IF_ERROR( iog_stack_free_rest(stack) );
  }

  return OK;
}


/**
 * @param[out] stack pointer to stack (can't be NULL)
 * @param[out] value pointer to variable in which want to write
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
IogStackReturnCode iog_stack_peek (const IogStack_t *stack, iog_stack_value_t *value) {
  IOG_ASSERT(value);

  IOG_RETURN_IF_ERROR( iog_stack_verify(stack) );

  if (stack->size == 0)
    return ERR_STACK_UNDERFLOW;
   
  *value = stack->data[stack->size-1];

  IOG_RETURN_IF_ERROR( iog_stack_verify(stack) );

  return OK;
}


/**
 * @param[in]  stack         pointer to stack
 * @param[out] stream        pointer to stream for prints
 * @param[in]  stk_name      name of dumping stack
 * @param[in]  file_name     name of file from that called dump
 * @param[in]  line_num      number of line from that called dump
 * @param[in]  function_name name of function from that called dump
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
IogStackReturnCode iog_stack_dump_f (const IogStack_t *stack, FILE *stream,
      const char *stk_name, const char *file_name, int line_num, const char *function_name) {
  IOG_ASSERT(stream);


  fprintf(stream, BLACK("------------ STACK DUMP ------------" "\n"));
  fprintf(stream, BLUE("Called from %s:%d: %s\n"),
     file_name, line_num, function_name
  );

  if (stack == NULL) {
    fprintf(stream, BLACK("IogStack_t %s (null)") " {}\n", stk_name);
    return ERR_STACK_NULLPTR;
  }

  fprintf(stream, BLACK("IogStack_t %s (%p) {\n"), stk_name, stack);

  fprintf(stream, BLACK("  .firstStackCanary  = 0x%llx")  "\n",  stack->firstStackCanary);
  fprintf(stream, BLACK("   (canary - const   = 0x%llx)") "\n",
      stack->firstStackCanary - STACK_CANARY_CONST
  );

  fprintf(stream, BLACK("  .isInitialized     = %d")    "\n",  (int) stack->isInitialized);
  fprintf(stream, BLACK("  .size              = %lu")   "\n",  stack->size);
  fprintf(stream, BLACK("  .capacity          = %lu")   "\n",  stack->capacity);

  fprintf(stream, BLACK("  .firstDataCanary  = %p")  "\n",  stack->firstDataCanary);
  if (stack->firstDataCanary != NULL) {
    fprintf(stream, BLACK("  *firstDataCanary  = 0x%llx")  "\n",  *stack->firstDataCanary);
    fprintf(stream, BLACK("   (canary - const  = 0x%llx)") "\n",
        *stack->firstDataCanary - DATA_CANARY_CONST
    );
  }

  fprintf(stream, BLACK("  .data[%lu] (%p)" " = ["),           stack->capacity, stack->data);

  if (stack->data != NULL) {
    fprintf(stream, "\n");
    for (size_t i = 0; i < stack->capacity; i++)
      fprintf(stream, BLACK("    [%lu]: %lg\n"), i, stack->data[i]);
  }

  fprintf(stream, BLACK("  ]\n"));

  fprintf(stream, BLACK("  .secondDataCanary  = %p")  "\n",  stack->secondDataCanary);
  if (stack->secondDataCanary != NULL) {
    fprintf(stream, BLACK("  *secondDataCanary  = 0x%llx")  "\n",  *stack->secondDataCanary);
    fprintf(stream, BLACK("   (canary - const   = 0x%llx)") "\n",
       *stack->secondDataCanary - DATA_CANARY_CONST
    );
  }

  fprintf(stream, BLACK("  .secondStackCanary = 0x%llx")  "\n",  stack->secondStackCanary);
  fprintf(stream, BLACK("   (canary - const   = 0x%llx)") "\n",
      stack->secondStackCanary - STACK_CANARY_CONST
  );

  fprintf(stream, BLACK("}\n"));

  fprintf(stream, BLACK("------------------------------------\n"));

  return OK;
}

/**
 * @param[in] stack pointer to stack
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
IogStackReturnCode iog_stack_dump (const IogStack_t *stack) {
  return iog_stack_dump_f(stack, stdout, NULL, NULL, 0, NULL);
}


/**
 * Checks nullptrs, overflowing, intialization.
 * @param[in] stack pointer to stack
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
IogStackReturnCode iog_stack_verify (const IogStack_t *stack) {
  IOG_CHECK_STACK_NULL( stack );

  if ((stack->firstStackCanary - STACK_CANARY_CONST) != (iog_canary_t) stack)
    return ERR_DEAD_FIRST_CANARY;

  if ((stack->secondStackCanary - STACK_CANARY_CONST) != (iog_canary_t) stack)
    return ERR_DEAD_SECOND_CANARY;

  if (!stack->isInitialized)
    return ERR_STACK_ISNT_INITIALIZED;
  
  if (stack->size > stack->capacity)
    return ERR_STACK_OVERFLOW;

  if (stack->capacity < INIT_STACK_DATA_CAPACITY)
    return ERR_STACK_CAPACITY_UNDERFLOW;

  if (stack->data == NULL)
    return ERR_STACK_DATA_NULLPTR;

  if (stack->firstDataCanary == NULL)
    return ERR_FIRST_DATA_CANARY_NULLPTR;

  if (stack->secondDataCanary == NULL)
    return ERR_SECOND_DATA_CANARY_NULLPTR;

  //fprintf(stderr, BLACK("-- verify: %llx %llx\n"), (*stack->firstDataCanary - DATA_CANARY_CONST), (iog_canary_t) stack->data);
  if ((*stack->firstDataCanary - DATA_CANARY_CONST) != (iog_canary_t) stack->data)
    return ERR_DEAD_FIRST_DATA_CANARY;

  if ((*stack->secondDataCanary - DATA_CANARY_CONST) != (iog_canary_t) stack->data)
    return ERR_DEAD_SECOND_DATA_CANARY;


  return OK;
}

/**
 * @param[in] stack pointer to stack
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
IogStackReturnCode iog_stack_update_canaries (IogStack_t *stack) {
  IOG_CHECK_STACK_NULL( stack );

  stack->firstStackCanary  = STACK_CANARY_CONST + (iog_canary_t) stack;
  stack->secondStackCanary = STACK_CANARY_CONST + (iog_canary_t) stack;

  if (stack->data == NULL)
    return ERR_STACK_DATA_NULLPTR;

  if (stack->firstDataCanary == NULL)
    return ERR_FIRST_DATA_CANARY_NULLPTR;

  if (stack->secondDataCanary == NULL)
    return ERR_SECOND_DATA_CANARY_NULLPTR;

  
  *stack->firstDataCanary  = DATA_CANARY_CONST + (iog_canary_t) stack->data;
  *stack->secondDataCanary = DATA_CANARY_CONST + (iog_canary_t) stack->data;

  return OK;
}

//--------------------- PRIVATE FUNCTIONS --------------------------------------------

/**
 * Allocates max( new_capacity, INIT_STACK_DATA_CAPACITY ).
 * Can't free data, for that use destroy.
 * @param[in] stack        pointer to stack
 * @param[in] new_capacity new capacity of stack data
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
static IogStackReturnCode iog_stack_allocate_data (IogStack_t *stack, size_t new_capacity) {
  IOG_CHECK_STACK_NULL(stack);

  if (new_capacity < INIT_STACK_DATA_CAPACITY)
      new_capacity = INIT_STACK_DATA_CAPACITY;

  if (stack->firstDataCanary != NULL)
    *stack->firstDataCanary = 0;

  if (stack->secondDataCanary != NULL)
    *stack->secondDataCanary = 0;

  iog_canary_t *tmp_ptr = (iog_canary_t *) iog_recalloc (
      stack->firstDataCanary,
      2 * sizeof(iog_canary_t) + stack->capacity * sizeof(iog_stack_value_t),
      2 * sizeof(iog_canary_t) + new_capacity * sizeof(iog_stack_value_t),
      1
  );

  if (tmp_ptr == NULL)
    return ERR_CANT_ALLOCATE_DATA;

  stack->firstDataCanary = tmp_ptr;
  stack->data = (iog_stack_value_t *) (tmp_ptr + 1);
  stack->secondDataCanary = (iog_canary_t *) (stack->data + new_capacity);

  stack->capacity = new_capacity;

  return OK;
}

/**
 * Double capacity by allocating more memory
 * @param[in] stack pointer to stack
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
static IogStackReturnCode iog_stack_allocate_more (IogStack_t *stack) {
  IOG_RETURN_IF_ERROR( iog_stack_verify(stack) );

  if (iog_stack_allocate_data(stack, stack->capacity * 2) != OK)
    return ERR_CANT_ALLOCATE_DATA;

  iog_stack_update_canaries(stack);

  IOG_RETURN_IF_ERROR( iog_stack_verify(stack) );

  return OK;
}

/**
 * @param[in] stack pointer to stack
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
static IogStackReturnCode iog_stack_free_rest (IogStack_t *stack) {
  IOG_RETURN_IF_ERROR (iog_stack_verify(stack) );

  if (iog_stack_allocate_data(stack, stack->size) != OK)
    return ERR_CANT_FREE_DATA;

  iog_stack_update_canaries(stack);

  IOG_RETURN_IF_ERROR( iog_stack_verify(stack) );

  return OK;
}
