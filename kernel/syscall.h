// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_SYSCALL_H
#define MORDAX_SYSCALL_H

#include "syscalls.h"
#include "types.h"

/**
 * @defgroup syscalls System Call Support
 * @{
 */

/**
 * System syscall handler.
 * @param context process context information.
 */
void syscall_system(struct thread_context * context, int function);

/**
 * Thread exit syscall handler. Takes the exit status as parameter.
 * @param context process context information.
 */
void syscall_thread_exit(struct thread_context * context);

/**
 * Thread create syscall handler. Takes two parameters,
 * the entry point of the new thread and the top of the stack
 * for the new thread.
 * @param context process context information.
 */
tid_t syscall_thread_create(struct thread_context * context);

/**
 * Thread join syscall handler. Takes the TID of the thread
 * to join as parameter and returns the return value of the
 * thread, or -1 if the thread ID did not exist.
 * @todo This syscall returns -1 even if the thread previously existed.
 * @param context process context information.
 */
void syscall_thread_join(struct thread_context * context);

/** @} */

#endif

