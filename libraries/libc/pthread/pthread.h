// The Mordax POSIX Thread Emulation Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef __MORDAX_LIBC_PTHREAD_H__
#define __MORDAX_LIBC_PTHREAD_H__

#include <sys/types.h>

#define PTHREAD_MUTEX_INITIALIZER	0

/**
 * Creates a new thread.
 * @param thread location where the identifier of the thread should be stored.
 * @param attr thread attributes. If this is `NULL`, default attributes are used.
 * @param start_routine entrypoint for the new thread.
 * @param arg argument passed to the thread entry function.
 * @return 0 if successful, otherwise an error code is returned.
 */
int pthread_create(pthread_t * restrict thread, const pthread_attr_t * restrict attr,
	void * (*start_routine)(void *), void * restrict arg);

/**
 * Joins with a terminated thread.
 * @param thread ID of the thread to join with.
 * @param retval pointer to where the exit code of the terminated thread should be stored.
 * @return 0 on success, an error code on error.
 */
int pthread_join(pthread_t thread, void ** retval);

/**
 * Locks a mutex.
 * @param mutex a pointer to the mutex to lock.
 * @return 0 on success, an error code on error.
 */
int pthread_mutex_lock(pthread_mutex_t * mutex);

/**
 * Unlocks a mutex.
 * @param mutex a pointer to the mutex to unlock.
 * @return 0 on success, an error code on error.
 */
int pthread_mutex_unlock(pthread_mutex_t * mutex);

#endif

