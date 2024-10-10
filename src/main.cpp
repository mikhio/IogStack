#include <stdio.h>

#include "iog_stack.h"
#include "cli_colors.h"
#include "iog_stack_tests.h"

#define IOG_ERR(func) {                                                  \
  ReturnCode err = func;                                                 \
  if (err != OK)                                                         \
      fprintf(stderr, RED("line %d: " #func " = %d\n"), __LINE__, func); \
}

int main(const int argc, const char *argv[]) {
  IogStack_t stk = {};
  

  return 0;
}
