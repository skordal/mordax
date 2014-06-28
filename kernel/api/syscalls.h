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
#define MORDAX_SYSCALL_SOCKET_WAIT	15
#define MORDAX_SYSCALL_SOCKET_CLOSE	16

// Lock syscalls:
#define MORDAX_SYSCALL_LOCK_CREATE	17
#define MORDAX_SYSCALL_LOCK_AQUIRE	18
#define MORDAX_SYSCALL_LOCK_RELEASE	19

// Device tree syscalls:
#define MORDAX_SYSCALL_DT_GET_NODE_BY_PATH		20
#define MORDAX_SYSCALL_DT_GET_NODE_BY_PHANDLE		21
#define MORDAX_SYSCALL_DT_GET_NODE_BY_COMPATIBLE	22
#define MORDAX_SYSCALL_DT_GET_PROPERTY_ARRAY32		23
#define MORDAX_SYSCALL_DT_GET_PROPERTY_STRING		24
#define MORDAX_SYSCALL_DT_GET_PROPERTY_PHANDLE		25

// IRQ syscalls:
#define MORDAX_SYSCALL_IRQ_CREATE	26
#define MORDAX_SYSCALL_IRQ_LISTEN	27

// Resource syscalls:
#define MORDAX_SYSCALL_RESOURCE_DESTROY	28

#endif

