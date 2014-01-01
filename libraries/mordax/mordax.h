// The Mordax System Call Library
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef __MORDAX_H__
#define __MORDAX_H__

#include <private/memory.h>
#include <private/process.h>
#include <private/system.h>
#include <private/types.h>

/**
 * System administration system call.
 * The first parameter is a function, defined in `private/system.h`,
 * and the rest of the parameters are additional arguments for the
 * requested function.
 * @note This should not be used in normal applications.
 */
void * mordax_system();

/**
 * Exits a thread.
 * @param exit_code exit code, returned to any waiting threads.
 */
void mordax_thread_exit(int exit_code) __attribute((noreturn));

/**
 * Creates a new thread.
 * @param entry entry point of the thread.
 * @param stack pointer to the _top_ of the stack for the new thread.
 * @return the thread ID of the newly created thread or `-1` if an error occured.
 */
tid_t mordax_thread_create(void * entry, void * stack);

/**
 * Waits for a thread to exit.
 * @param tid the thread ID of the thread to wait for.
 * @return the exit code of the thread, or `-1` if an error occured.
 */
int mordax_thread_join(tid_t tid);

/**
 * Gets information about the current thread/process.
 * @param function specifies which information to return. The valid constants
 *                 for this parameter are defined in `private/thread.h`.
 * @return the requested information.
 */
void * mordax_thread_info(int function);

/**
 * Creates a new process.
 * @param procinfo information about the new process to create.
 * @return the PID of the new process or -1 if an error occurs.
 */
pid_t mordax_process_create(struct mordax_process_info * procinfo);

#endif

