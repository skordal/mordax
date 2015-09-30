// The Mordax Standard Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef __MORDAX_LIBC_STDIO_H__
#define __MORDAX_LIBC_STDIO_H__

#include <stdarg.h>
#include <sys/types.h>

#define MPRINTF_MAXLEN	256

/**
 * Prints a formatted string using the `mordax_system` syscall.
 */
int mprintf(char * restrict format, ...);

/**
 * Prints a formatted string.
 */
int vsnprintf(char * restrict s, size_t size, const char * restrict format, va_list arguments);

/**
 * Prints a formatted string.
 */
int snprintf(char * restrict s, size_t size, const char * restrict format, ...);

#endif

