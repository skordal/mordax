// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_API_SYSCALLS_H
#define MORDAX_API_SYSCALLS_H

/*
 * This file contains a list of the system calls supported by the kernel.
 * A description of each system call can be found in the `mordax.h` header
 * of the Mordax library.
 */

#define MORDAX_SYSCALL_SYSTEM		0
#define MORDAX_SYSCALL_THREAD_EXIT	1
#define MORDAX_SYSCALL_THREAD_CREATE	2
#define MORDAX_SYSCALL_THREAD_JOIN	3
#define MORDAX_SYSCALL_THREAD_INFO	4
#define MORDAX_SYSCALL_PROCESS_CREATE	5
#define MORDAX_SYSCALL_MAP		6
#define MORDAX_SYSCALL_UNMAP		7

#endif

