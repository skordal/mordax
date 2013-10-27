// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_THREAD_H
#define MORDAX_THREAD_H

#include "context.h"
#include "types.h"

/**
 * @defgroup thread Thread Support
 * @{
 */

/**
 * Thread structure.
 *
 * Mordax processes consists of one or more threads. All threads share the same
 * address space, which is maintained by the `process` structure.
 *
 * @see process
 */
struct thread
{
	struct thread_context * context;	//< Stored context for this thread.
	struct process * parent;		//< Parent process of this thread.
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

/** @} */

#endif

