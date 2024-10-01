#ifndef IOG_MEMLIB_H
#define IOG_MEMLIB_H

/** @file iog_memlib.h */

/// Reallocate memory
void *iog_recalloc(void *ptr, size_t old_num, size_t new_num, size_t elem_size);

#endif // IOG_MEMLIB_H
