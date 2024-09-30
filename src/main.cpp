#include <stdio.h>

#include "iog_stack.h"
#include "cli_colors.h"

#define IOG_ERR(func) {                                                  \
  ReturnCode err = func;                                                 \
  if (err != OK)                                                         \
      fprintf(stderr, RED("line %d: " #func " = %d\n"), __LINE__, func); \
}

int main(const int argc, const char *argv[]) {
  IogStack_t stk = {};

  IOG_ERR( iog_stack_init(&stk) );
  IOG_ERR( iog_stack_dump(&stk) );

  IOG_ERR( iog_stack_push(&stk, 8.5) );
  IOG_ERR( iog_stack_push(&stk, 9.5) );
  IOG_ERR( iog_stack_dump(&stk) );

  iog_stack_value_t test_value = 0;

  IOG_ERR( iog_stack_peek (&stk, &test_value) );
  printf("test_value (peek) = %lg\n", test_value);

  IOG_ERR( iog_stack_pop  (&stk, &test_value) );
  printf("test_value (pop1) = %lg\n", test_value);

  IOG_ERR( iog_stack_pop  (&stk, &test_value) );
  printf("test_value (pop2) = %lg\n", test_value);

  IOG_ERR( iog_stack_dump (&stk) );

  IOG_ERR( iog_stack_destroy(&stk) );
  IOG_ERR( iog_stack_dump(&stk) );

  return 0;
}
