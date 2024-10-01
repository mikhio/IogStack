#include <stdlib.h>
#include <string.h>

#include "iog_memlib.h"

/**
 * Does new calloc, copies old number of elemnts from old pointer to new pointer and frees old pointer.
 * If old pointer is null, then will be just callocation.
 * If new number or size of elements is NULL, then old pointer wiil be released and NULL will be returned.
 * @param[in] ptr       old pointer to data
 * @param[in] old_num   old number of elements
 * @param[in] new_num   new number of elements
 * @param[in] elem_size size of one element in bytes
 * @return new pointer to allocated memory
 */
void *iog_recalloc (void *ptr, size_t old_num,  size_t new_num, size_t elem_size) {
  if ((new_num * elem_size) == 0) {
    free(ptr);
    return NULL;
  }

  void *new_ptr = calloc(new_num, elem_size);

  if (ptr != NULL) {
    if (old_num <= new_num)
      memcpy(new_ptr, ptr, old_num * elem_size);
    else
      memcpy(new_ptr, ptr, new_num * elem_size);
  }
  
  free(ptr);
  ptr = NULL;

  return new_ptr;
}
