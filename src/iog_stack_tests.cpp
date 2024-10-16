#include <stdio.h>

#include "iog_stack_tests.h"
#include "iog_stack.h"
#include "iog_stack_return_codes.h"
#include "cli_colors.h"


/**
 * @param[in] stack pointer to stack
 * @return Error code (if ok return IogStackReturnCode.OK)
 */
IogStackReturnCode iog_stack_canaries_check (const IogStack_t *stack) {
  iog_check_first_stack_canary(stack);
  iog_check_second_stack_canary(stack);

  return OK;
}

IogStackReturnCode iog_check_first_stack_canary(const IogStack_t *stack) {
  IogStackReturnCode err = iog_stack_verify(stack);
  if (err != OK) {
    fprintf(stderr, RED("Error: ") "stack is already broken with code: %d\n", err);
    return err;
  }

  IogStackCanaryTest_t ctest1 = {0, *stack, 0};
  iog_stack_update_canaries(&ctest1.stack);

  IogStackReturnCode firstTestErr = OK;
  for (size_t i = 0; i < 20; i++) {
    firstTestErr = iog_stack_verify(&ctest1.stack);
    if (firstTestErr == ERR_DEAD_FIRST_CANARY) {
      fprintf(stderr, GREEN("Canary died after %lu iterations\n"), i);
      break;
    }
    *(ctest1.firstTest + i) = 0;
  }

  if (firstTestErr != ERR_DEAD_FIRST_CANARY) {
    fprintf(stderr, RED("FIRST CANARY TEST FAILED, with verify code: %d\n"), firstTestErr);
    IOG_STACK_DUMP( &ctest1.stack );

    return ERR_TEST_FAILED;
  } else {
    fprintf(stderr, GREEN("FIRST CANARY TEST PASSED\n"));
    IOG_STACK_DUMP( &ctest1.stack );
  }

  return OK;
}

IogStackReturnCode iog_check_second_stack_canary(const IogStack_t *stack) {
  IogStackReturnCode err = iog_stack_verify(stack);
  if (err != OK) {
    fprintf(stderr, RED("Error: ") "stack is already broken with code: %d\n", err);
    return err;
  }

  IogStackCanaryTest_t ctest2 = {0, *stack, 0};
  iog_stack_update_canaries(&ctest2.stack);

  IogStackReturnCode secondTestErr = OK;
  for (size_t i = 0; i < 20; i++) {
    secondTestErr = iog_stack_verify(&ctest2.stack);
    if (secondTestErr == ERR_DEAD_SECOND_CANARY) {
      fprintf(stderr, GREEN("Second stack canary died after %lu iterations\n"), i);
      break;
    }
    *(ctest2.secondTest - i) = 0;
  }

  if (secondTestErr != ERR_DEAD_SECOND_CANARY) {
    fprintf(stderr, RED("SECOND STACK CANARY TEST FAILED, with verify code: %d\n"), secondTestErr);
    IOG_STACK_DUMP( &ctest2.stack );

    return ERR_TEST_FAILED;
  } else {
    fprintf(stderr, GREEN("SECOND STACK CANARY TEST PASSED\n"));
    IOG_STACK_DUMP( &ctest2.stack );
  }

  return OK;
}

IogStackReturnCode iog_check_first_data_canary() {
  return OK;
}

IogStackReturnCode iog_check_second_data_canary() {
  return OK;
}
