#include <stdio.h>

#include "iog_stack.h"
#include "cli_colors.h"
#include "iog_stack_tests.h"
#include "iog_assert.h"

#define IOG_ERR(func) {                                                  \
  IogStackReturnCode err = func;                                                 \
  if (err != OK)                                                         \
      fprintf(stderr, RED("line %d: " #func " = %d\n"), __LINE__, err);  \
}

int main(const int argc, const char *argv[]) {
  IogStack_t stk = {};

  IOG_ERR( iog_stack_init(&stk) );

  IOG_STACK_DUMP(&stk);

  IOG_ERR( iog_stack_push(&stk, 8.5) );
  IOG_ERR( iog_stack_push(&stk, 9.5) );
  IOG_ERR( iog_stack_push(&stk, 9.5) );
  IOG_ERR( iog_stack_push(&stk, 9.5) );
  IOG_ERR( iog_stack_push(&stk, 9.5) );
  IOG_STACK_DUMP(&stk);

  iog_stack_value_t test_value = 0;

  IOG_ERR( iog_stack_peek (&stk, &test_value) );
  printf("test_value (peek) = %lg\n", test_value);
  IOG_ERR( iog_stack_pop  (&stk, &test_value) );
  printf("test_value (pop)  = %lg\n", test_value);
  IOG_STACK_DUMP (&stk);


  printf(MAGENTA("---------------- START TESTS -----------------\n"));

  iog_stack_canaries_check(&stk);

  printf(MAGENTA("---------------- END TESTS -----------------\n"));

  IOG_ERR( iog_stack_destroy(&stk) );
  IOG_STACK_DUMP(&stk);


  return 0;
}
