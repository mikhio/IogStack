#include <stdlib.h>

#include "iog_assert.h"
#include "iog_stack.h"
#include "cli_colors.h"
#include "iog_memlib.h"

//--------------------- PUBLIC FUNCTIONS --------------------------------------------

/**
 * Allocates stack data with INIT_STACK_DATA_CAPACITY size and null values.
 * Turns isInitialized flag to 1.
 * @param[out] stack pointer to stack (can't be NULL)
 * @return Error code (if ok return ReturnCode.OK)
 */
ReturnCode iog_stack_init(IogStack_t *stack) {
  IOG_CHECK_STACK_NULL( stack );

  if (stack->isInitialized)
    return ERR_STACK_ALREADY_INITIALIZED;

  iog_stack_value_t *tmp_data = (iog_stack_value_t *) calloc(INIT_STACK_DATA_CAPACITY, sizeof (iog_stack_value_t));

  if (tmp_data == NULL)
    return ERR_CANT_ALLOCATE_DATA;

  stack->data = tmp_data;
  stack->size = 0;
  stack->capacity = INIT_STACK_DATA_CAPACITY;
  stack->isInitialized = 1;


  IOG_RETURN_IF_ERROR( iog_stack_verify(stack) );

  return OK;
}

/**
 * Frees data and reset stack to zero
 * @param[out] stack pointer to stack (can't be NULL)
 * @return Error code (if ok return ReturnCode.OK)
 */
ReturnCode iog_stack_destroy(IogStack_t *stack) {
  IOG_CHECK_STACK_NULL( stack );

  free(stack->data);

  stack->data = 0;
  stack->size = 0;
  stack->capacity = 0;
  stack->isInitialized = 0;

  return OK;
}

/**
 * Adds value to the end of data array and increments size
 * @param[out] stack pointer to stack (can't be NULL)
 * @param[in]  value new stack value
 * @return Error code (if ok return ReturnCode.OK)
 */
ReturnCode iog_stack_push (IogStack_t *stack, iog_stack_value_t value) {
  IOG_RETURN_IF_ERROR( iog_stack_verify(stack) );

  if (stack->size == stack->capacity) {
    IOG_RETURN_IF_ERROR( iog_stack_allocate_more(stack) );
  }

  stack->data[stack->size] = value;
  stack->size++;

  return OK;
}

/**
 * If size less 1/4 of capacity then frees rest memory.
 * @param[out] stack pointer to stack (can't be NULL)
 * @param[out] value pointer to variable in which want to write
 * @return Error code (if ok return ReturnCode.OK)
 */
ReturnCode iog_stack_pop (IogStack_t *stack, iog_stack_value_t *value) {
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
 * @return Error code (if ok return ReturnCode.OK)
 */
ReturnCode iog_stack_peek (const IogStack_t *stack, iog_stack_value_t *value) {
  IOG_ASSERT(value);

  IOG_RETURN_IF_ERROR( iog_stack_verify(stack) );

  if (stack->size == 0)
    return ERR_STACK_UNDERFLOW;
   
  *value = stack->data[stack->size-1];

  return OK;
}


/**
 * @param[in]  stack         pointer to stack
 * @param[out] stream        pointer to stream for prints
 * @param[in]  stk_name      name of dumping stack
 * @param[in]  file_name     name of file from that called dump
 * @param[in]  line_num      number of line from that called dump
 * @param[in]  function_name name of function from that called dump
 * @return Error code (if ok return ReturnCode.OK)
 */
ReturnCode iog_stack_dump_f (const IogStack_t *stack, FILE *stream,
      const char *stk_name, const char *file_name, int line_num, const char *function_name) {
  IOG_ASSERT(stream);


  fprintf(stream, BLACK("------- STACK DUMP ---------" "\n"));
  fprintf(stream, BLUE("Called from %s:%d: %s\n"),
     file_name, line_num, function_name
  );

  if (stack == NULL) {
    fprintf(stream, BLACK("IogStack_t %s (null)") " {}\n", stk_name);
    return ERR_STACK_NULLPTR;
  }

  fprintf(stream, BLACK("IogStack_t %s (%p)") " {\n", stk_name, stack);

  fprintf(stream, "  .isInitialized  = %d"  "\n",  (int) stack->isInitialized);
  fprintf(stream, "  .size           = %lu" "\n",  stack->size);
  fprintf(stream, "  .capacity       = %lu" "\n",  stack->capacity);
  fprintf(stream, "  .data[%lu] (%p) = [",         stack->capacity, stack->data);

  if (stack->data != NULL) {
    fprintf(stream, "\n");
    for (size_t i = 0; i < stack->capacity; i++)
      fprintf(stream, "    [%lu]: %lg\n", i, stack->data[i]);
  }

  fprintf(stream, "  ]\n");
  fprintf(stream, "}\n");

  return OK;
}

/**
 * @param[in] stack pointer to stack
 * @return Error code (if ok return ReturnCode.OK)
 */
ReturnCode iog_stack_dump (const IogStack_t *stack) {
  return iog_stack_dump_f(stack, stdout, NULL, NULL, 0, NULL);
}


/**
 * Checks nullptrs, overflowing, intialization.
 * @param[in] stack pointer to stack
 * @return Error code (if ok return ReturnCode.OK)
 */
ReturnCode iog_stack_verify (const IogStack_t *stack) {
  IOG_CHECK_STACK_NULL( stack );

  if (!stack->isInitialized)
    return ERR_STACK_ISNT_INITIALIZED;
  
  if (stack->size > stack->capacity)
    return ERR_STACK_OVERFLOW;

  if (stack->capacity < INIT_STACK_DATA_CAPACITY)
    return ERR_STACK_CAPACITY_UNDERFLOW;

  if (stack->data == NULL)
    return ERR_STACK_DATA_NULLPTR;

  return OK;
}

//--------------------- PRIVATE FUNCTIONS --------------------------------------------

/**
 * Double capacity by allocating more memory
 * @param[out] stack pointer to stack
 * @return Error code (if ok return ReturnCode.OK)
 */
static ReturnCode iog_stack_allocate_more (IogStack_t *stack) {
  IOG_RETURN_IF_ERROR( iog_stack_verify(stack) );

  iog_stack_value_t *tmp_ptr = (iog_stack_value_t *) iog_recalloc (
      stack->data, stack->capacity, stack->capacity * 2, sizeof (iog_stack_value_t)
  );
  if (tmp_ptr == NULL)
    return ERR_CANT_ALLOCATE_DATA;

  stack->data = tmp_ptr;
  stack->capacity *= 2;

  return OK;
}

/**
 * @param[out] stack pointer to stack
 * @return Error code (if ok return ReturnCode.OK)
 */
static ReturnCode iog_stack_free_rest (IogStack_t *stack) {
  IOG_RETURN_IF_ERROR (iog_stack_verify(stack) );

  size_t new_capacity = stack->size;

  if (stack->size < INIT_STACK_DATA_CAPACITY)
    new_capacity = INIT_STACK_DATA_CAPACITY;


  if (stack->capacity > new_capacity) {
    stack->data = (iog_stack_value_t *) iog_recalloc (
       stack->data, stack->capacity, new_capacity, sizeof (iog_stack_value_t)
    );

    stack->capacity = new_capacity;
  }


  return OK;
}
