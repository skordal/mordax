// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_THREAD_H
#define MORDAX_THREAD_H

#include "context.h"
#include "queue.h"
#include "types.h"

/**
 * @defgroup thread Thread Support
 * @{
 */

struct context;

/**
 * Thread structure.
 *
 * Mordax processes consists of one or more threads. All threads share the same
 * address space, which is maintained by the `process` structure.
 *
 * The thread structure contains a field called `exit_listeners`, which is a list
 * of threads currently blocking and waiting for the thread to exit. When the thread
 * is exited, the waiting threads are moved from the blocking queue to the running
 * queue.
 *
 * @see process
 */
struct thread
{
	struct thread_context * context;	//< Stored context for this thread.
	struct process * parent;		//< Parent process of this thread.
	struct queue * exit_listeners;		//< List of threads waiting for this thread to exit.
	tid_t tid;				//< PID of this thread (more like thread ID).
};

/**
 * Creates a thread.
 * @param process the process to create the thread in.
 * @param entrypoint entry-point address for the process.
 * @param stack virtual address to the top of the stack to use for the thread.
 * @return a pointer to the new thread object.
 */
struct thread * thread_create(struct process * parent, void * entrypoint, void * stack)
	__attribute((malloc));

/**
 * Frees a thread. The thread must have been removed from the
 * scheduler, or problems will result.
 * @param t the thread to free.
 * @oaram retval thread exit status.
 */
void thread_free(struct thread * t, int retval);

/**
 * Adds a thread to the list of exit listeners.
 * @param t the thread to add the listener to.
 * @param l the thread that listens.
 */
void thread_add_exit_listener(struct thread * t, struct thread * l);

/** @} */

#endif

