#ifndef IOG_STACK_H
#define IOG_STACK_H

#include "return_codes.h"

/* @file iog_stack.h */

/// Macros for calling functions and return error if isn't OK.
#define IOG_RETURN_IF_ERROR(func) { \
  ReturnCode err = func;            \
  if (err != OK)                    \
    return err;                     \
}    

/// Macros returns error code if stack is nullptr 
#define IOG_CHECK_STACK_NULL(stack) {  \
  if (stack == NULL)                   \
    return ERR_STACK_NULLPTR;          \
}

/// Macros calls dump function with extra information about calling. 
#define IOG_STACK_DUMP(stack) {                                                     \
  iog_stack_dump_f(stack, stdout, #stack, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
}

typedef double iog_stack_value_t; ///< Definition of stack element type
typedef unsigned char iog_flag_t; ///< Definition of flag type;
                                  
const size_t INIT_STACK_DATA_CAPACITY = 4; ///< Constant with init capacity 

/** @struct IogStack_t
 * Defines stack structure
 */
struct IogStack_t {
  iog_stack_value_t *data;  ///< Pointer to array with data
  iog_flag_t isInitialized; ///< Flag of initialization
  size_t size;              ///< Amount of valuable elements in data
  size_t capacity;          ///< Size of allocated memory for data
};

//--------------------- PUBLIC FUNCTIONS --------------------------------------------

ReturnCode iog_stack_init    (IogStack_t *stack); ///< Initialize stack
ReturnCode iog_stack_destroy (IogStack_t *stack); ///< Free stack data from memory

ReturnCode iog_stack_push (IogStack_t *stack, iog_stack_value_t value);  ///< Add value to stack
ReturnCode iog_stack_pop  (IogStack_t *stack, iog_stack_value_t *value); ///< Read and remove value from stack

ReturnCode iog_stack_peek (const IogStack_t *stack, iog_stack_value_t *value); ///< Read value from stack
                                                                               
/// Print all stack info to stream (file)
ReturnCode iog_stack_dump_f (const IogStack_t *stack, FILE *stream,
    const char *stk_name, const char *file_name, int line_num, const char *function_name);  
                                                                     
ReturnCode iog_stack_dump   (const IogStack_t *stack);               ///< Print all stack info to stdin 
ReturnCode iog_stack_verify (const IogStack_t *stack);               ///< Verify stack



//--------------------- PRIVATE FUNCTIONS --------------------------------------------

static ReturnCode iog_stack_allocate_more (IogStack_t *stack); ///< Allocates more memory for data
static ReturnCode iog_stack_free_rest     (IogStack_t *stack); ///< Free all memory after stack size.

#endif // IOG_STACK_H
