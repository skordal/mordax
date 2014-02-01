// The Mordax POSIX Thread Emulation Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <errno.h>
#include <rbtree.h>
#include <stdlib.h>
#include <string.h>

#include <mordax.h>
#include "pthread.h"

#define THREAD_STACK_SIZE	4096*16

// Actual thread entrypoint, which makes sure arguments are passed correctly to the
// entry-point function. See the armv7/pthread_shim.S file for more information about
// how this works.
extern void __pthread_shim(void);

// Three holding all threads in the system:
static struct rbtree * threads;
static pthread_mutex_t threads_mutex = 0;
static bool initialized = false;

// Initialization function, initializes the pthreads subsystem:
static void pthreads_initialize(void)
{
	threads = rbtree_new(0, 0, 0, free);
	initialized = true;
}

// Function called when a thread is terminating:
void __attribute((noreturn)) __pthread_terminate(void * exit_code)
{
	pthread_mutex_lock(&threads_mutex);
	pthread_attr_t * thread_attributes = rbtree_get_value(threads, (void *) mordax_thread_info(MORDAX_THREAD_INFO_GET_TID));
	pthread_mutex_unlock(&threads_mutex);

	while(thread_attributes == 0)
	{
		// This condition can occur if a thread terminates before the thread that created it has inserted its
		// information into the thread table. In that case, make the kernel reschedule, and try again the next
		// time this thread is scheduled.
		mordax_thread_yield();

		pthread_mutex_lock(&threads_mutex);
		thread_attributes = rbtree_get_value(threads, (void *) mordax_thread_info(MORDAX_THREAD_INFO_GET_TID));
		pthread_mutex_unlock(&threads_mutex);
	}

	thread_attributes->exit_code = exit_code;
	thread_attributes->running = false;

	mordax_thread_exit((int) exit_code);
}

int pthread_create(pthread_t * restrict thread, const pthread_attr_t * restrict attr,
	void * (*start_routine)(void *), void * restrict arg)
{
	if(!initialized)
		pthreads_initialize();

	pthread_attr_t * attributes = malloc(sizeof(pthread_attr_t));

	if(attr == NULL)
	{
		attributes->stackaddr = malloc(THREAD_STACK_SIZE);
		attributes->stacksize = THREAD_STACK_SIZE;
	} else
		memcpy(attributes, attr, sizeof(pthread_attr_t));

	// TODO: Properly allocate thread stacks.
	uint32_t * thread_stack = attributes->stackaddr;
	thread_stack[(THREAD_STACK_SIZE / sizeof(uint32_t)) - 1] = (uint32_t) start_routine;
	thread_stack[(THREAD_STACK_SIZE / sizeof(uint32_t)) - 2] = (uint32_t) arg;

	attributes->thread_id = mordax_thread_create(__pthread_shim, (void *) ((uint32_t) thread_stack + THREAD_STACK_SIZE));

	if(attributes->thread_id >= 0)
	{
		attributes->running = true;
		*thread = attributes->thread_id;

		pthread_mutex_lock(&threads_mutex);
		rbtree_insert(threads, (void *) attributes->thread_id, attributes);
		pthread_mutex_unlock(&threads_mutex);
	}

	return attributes->thread_id >= 0 ? 0 : -attributes->thread_id;
}

int pthread_join(pthread_t thread, void ** retval)
{
	if(!initialized)
		pthreads_initialize();

	pthread_mutex_lock(&threads_mutex);
	volatile pthread_attr_t * thread_attributes = rbtree_get_value(threads, (void *) thread);
	pthread_mutex_unlock(&threads_mutex);

	if(thread_attributes == NULL)
		return ESRCH;

	if(thread_attributes->running)
		mordax_thread_join(thread_attributes->thread_id);

	if(retval != NULL)
		*retval = thread_attributes->exit_code;

	return 0;
}

int pthread_mutex_lock(pthread_mutex_t * mutex)
{
	if(!initialized)
		pthreads_initialize();

	// TODO: add architecture specific versions of these functions.
	while(*mutex == 1)
		mordax_thread_yield();
	*mutex = 1;

	return 0;
}

int pthread_mutex_unlock(pthread_mutex_t * mutex)
{
	if(!initialized)
		pthreads_initialize();

	// TODO: add architecture specific versions of these functions.
	*mutex = 0;
	return 0;
}

