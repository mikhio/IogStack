#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "iog_assert.h"
#include "cli_colors.h"

/**
 * Printf error and call exit(1)
 * @param[in] flag      if flag is 1 then make assert
 * @param[in] expr      string with checking expression
 * @param[in] line      number of expression line
 * @param[in] function  name of function with expression
 */
void iog_assert_func(int flag, const char *expr,
    const char *file, unsigned int line, const char *function)
{
  if (flag) {
    fprintf(stderr, RED("IOG_Assertion in %s: ") BLACK("%s") "\n"
        "  line %d:  " BLACK("%s") "\n"
        , file, function, line, expr);
    fprintf(stderr, "\n");
    exit(1);
  }

}

void iog_print_ram (void *start_ptr, size_t bytes_num) {
  fprintf(stderr, RED("---------- RAM ---------------\n"));

  for (size_t i = 0; i < bytes_num; i++) {
    unsigned char *cur_ptr = (unsigned char *)start_ptr + i;
    fprintf(stderr, BLACK("%p:") BLUE("  %X\n"), cur_ptr, *cur_ptr);
  }

  fprintf(stderr, RED("-----------------------------\n"));
}
