// The Mordax Standard Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef __MORDAX_LIBC_STRING_H__
#define __MORDAX_LIBC_STRING_H__

#include <sys/types.h>

void * memset(void * s, int c, size_t n);
void * memcpy(void * dest, const void * src, size_t n);

int strcmp(const char * s1, const char * s2);
int strncmp(const char * s1, const char * s2, size_t n);

size_t strlen(const char * s);

#endif

