// The Mordax Standard Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef __MORDAX_LIBC_STDLIB_H__
#define __MORDAX_LIBC_STDLIB_H__

#include <sys/types.h>

/**
 * Allocates memory.
 * @param size size of the chunk to allocate.
 * @return a pointer to the allocated memory or `NULL` if an error occurs.
 */
void * malloc(size_t size);

/**
 * Frees allocated memory.
 * @param ptr pointer to the allocated memory to free.
 */
void free(void * ptr);

#endif

