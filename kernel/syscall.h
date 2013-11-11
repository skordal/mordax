// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_SYSCALLS_H
#define MORDAX_SYSCALLS_H

#include "types.h"

/**
 * @defgroup syscalls System Call Support
 * @{
 */

/** System syscall. Used for various administrative purposes. */
#define SYSCALL_SYSTEM		0

/** Thread exit syscall. This exits and frees the calling thread. */
#define SYSCALL_THREAD_EXIT	1

/**
 * System syscall handler.
 * @param context process context information.
 */
void syscall_system(struct thread_context * context, int function);

/**
 * Thread exit syscall handler.
 * @param context process context information.
 */
void syscall_thread_exit(struct thread_context * context);

/** @} */

#endif

