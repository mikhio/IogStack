#ifndef IOG_STACK_TESTS_H
#define IOG_STACK_TESTS_H

#include "return_codes.h"
#include "iog_stack.h"

/** @file iog_stack_tests.h */

struct IogStackCanaryTest_t {
  char firstTest[1];
  IogStack_t stack;
  char secondTest[1];
};

ReturnCode iog_stack_canaries_check(const IogStack_t *stack);     ///< Test first and second canaries of stack

ReturnCode iog_check_first_stack_canary  (const IogStack_t *stack);
ReturnCode iog_check_second_stack_canary (const IogStack_t *stack);
ReturnCode iog_check_first_data_canary   (const IogStack_t *stack);
ReturnCode iog_check_second_data_canary  (const IogStack_t *stack);


#endif // IOG_STACK_TESTS_H
