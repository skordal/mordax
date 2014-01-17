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

#define MORDAX_SYSCALL_SYSTEM		 0

// Thread syscalls:
#define MORDAX_SYSCALL_THREAD_EXIT	 1
#define MORDAX_SYSCALL_THREAD_CREATE	 2
#define MORDAX_SYSCALL_THREAD_JOIN	 3
#define MORDAX_SYSCALL_THREAD_YIELD	 4
#define MORDAX_SYSCALL_THREAD_INFO	 5

// Process syscalls:
#define MORDAX_SYSCALL_PROCESS_CREATE	 6

// Memory management syscalls:
#define MORDAX_SYSCALL_MAP		 7
#define MORDAX_SYSCALL_MAP_ALLOC	 8
#define MORDAX_SYSCALL_UNMAP		 9

// IPC service syscalls:
#define MORDAX_SYSCALL_SERVICE_CREATE	10
#define MORDAX_SYSCALL_SERVICE_LISTEN	11
#define MORDAX_SYSCALL_SERVICE_CONNECT	12

// IPC socket syscalls:
#define MORDAX_SYSCALL_SOCKET_SEND	13
#define MORDAX_SYSCALL_SOCKET_RECEIVE	14
#define MORDAX_SYSCALL_SOCKET_CLOSE	15

// Resource syscalls:
#define MORDAX_SYSCALL_RESOURCE_DESTROY	16

#endif

