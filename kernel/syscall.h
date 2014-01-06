// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_SYSCALL_H
#define MORDAX_SYSCALL_H

#include "context.h"

/**
 * @defgroup syscalls System Calls
 * System call handlers.
 * @{
 */

/**
 * System syscall handler.
 * @param context process context information.
 */
void syscall_system(struct thread_context * context);

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
void syscall_thread_create(struct thread_context * context);

/**
 * Thread join syscall handler. Takes the TID of the thread
 * to join as parameter and returns the return value of the
 * thread, or -1 if the thread ID did not exist.
 * @todo This syscall returns -1 even if the thread previously existed.
 * @param context process context information.
 */
void syscall_thread_join(struct thread_context * context);

/**
 * Thread information syscall handler. Takes an integer parameter
 * specifying the information to return.
 */
void syscall_thread_info(struct thread_context * context);

/**
 * Process creation syscall handler. Takes a pointer to a filled in
 * mordax_process_info structure as parameter.
 */
void syscall_process_create(struct thread_context * context);

/**
 * Maps physical memory to virtual memory. Takes four arguments;
 * the target address, the source address, the size of the mapping
 * and a pointer to the desired attributes of the memory.
 */
void syscall_memory_map(struct thread_context * context);

/**
 * Allocates physical memory and maps it into virtual memory.
 * Takes three arguments; the target address, a pointer to the
 * size of the requested memory and a pointer to the desired
 * attributes of the memory. A pointer to the mapped memory is
 * returned from the syscall, as well as the size argument being
 * updated with the actual size allocated and mapped.
 */
void syscall_memory_map_alloc(struct thread_context * context);

/**
 * Unmaps mapped virtual memory. Takes two arguments; the address
 * to start unmapping at and the size of the area to unmap.
 */
void syscall_memory_unmap(struct thread_context * context);

/** @} */

#endif

