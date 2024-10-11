#ifndef RETURN_CODES_H
#define RETURN_CODES_H

/* @file return_codes.h */

/* @enum ReturnCode
 * Defines error code of function
 */
enum ReturnCode {
  OK                               = 0, ///< All ok

  ERR_STACK_ALREADY_INITIALIZED    = 1,
  ERR_STACK_ISNT_INITIALIZED       = 2,

  ERR_CANT_ALLOCATE_DATA           = 3,
  ERR_CANT_FREE_DATA               = 4,

  ERR_STACK_UNDERFLOW              = 5,
  ERR_STACK_OVERFLOW               = 6,
  ERR_STACK_CAPACITY_UNDERFLOW     = 7,

  ERR_STACK_NULLPTR                = 8,
  ERR_STACK_DATA_NULLPTR           = 9,

  ERR_DEAD_FIRST_CANARY            = 10,
  ERR_DEAD_SECOND_CANARY           = 11,

  ERR_DEAD_FIRST_DATA_CANARY       = 12,
  ERR_DEAD_SECOND_DATA_CANARY      = 13,

  ERR_FIRST_DATA_CANARY_NULLPTR    = 14,
  ERR_SECOND_DATA_CANARY_NULLPTR   = 15,

  ERR_TEST_FAILED                  = 16,

  NR_RETURN_CODE                   = 17, ///< Last return code
};

#endif // RETURN_CODES_H
