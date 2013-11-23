// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_API_SYSCALLS_H
#define MORDAX_API_SYSCALLS_H

/** System syscall. Used for various administrative purposes. */
#define MORDAX_SYSCALL_SYSTEM		0

/** Thread exit syscall. This exits and frees the calling thread. */
#define MORDAX_SYSCALL_THREAD_EXIT	1

/**
 * Thread creation syscall. This syscall takes two parameters,
 * a pointer to the entry point of the new thread and a pointer to
 * the top of the stack for the thread. It returns the thread
 * ID of the newly created thread, or -1 if an error occurs.
 */
#define MORDAX_SYSCALL_THREAD_CREATE	2

/**
 * Thread join syscall. Takes one parameter, the thread ID
 * of the thread to wait for. Blocks until the specified thread
 * terminates.
 */
#define MORDAX_SYSCALL_THREAD_JOIN	3

#endif

