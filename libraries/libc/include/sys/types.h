// The Mordax Standard Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef __MORDAX_LIBC_SYS_TYPES_H__
#define __MORDAX_LIBC_SYS_TYPES_H__

#include <mordax/types.h>

/** POSIX thread identifier type. */
typedef int pthread_t;

/** POSIX thread attribute type. */
typedef struct {
	void * stackaddr;
	size_t stacksize;
	tid_t thread_id;

	void * exit_code;
	bool running;
} pthread_attr_t;

/** POSIX mutex type. */
typedef volatile int pthread_mutex_t;

#endif

