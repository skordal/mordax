@ The Mordax System Call Library
@ (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

#include <private/syscalls.h>

.section .text

@ System syscall.
.global mordax_system
.type mordax_system, %function
mordax_system:
	svc #MORDAX_SYSCALL_SYSTEM
	bx lr

@ Exits a thread.
@ Parameters:
@ 	r0 = exit code
.global mordax_thread_exit
.type mordax_thread_exit, %function
mordax_thread_exit:
	svc #MORDAX_SYSCALL_THREAD_EXIT

1:	@ Loop code, in case the above falls through for some reason
	wfi
	b 1b

@ Creates a new thread.
@ Parameters:
@ 	r0 = thread entry-point
@ 	r1 = top of thread stack
@ Return values:
@ 	r0 = thread ID of the new thread or -1 on error
.global mordax_thread_create
.type mordax_thread_create, %function
mordax_thread_create:
	svc #MORDAX_SYSCALL_THREAD_CREATE
	bx lr

@ Joins with another thread.
@ Parameters:
@ 	r0 = ID of the thread to join with
@ Return values:
@ 	r0 = exit code of the thread or -1 on error
.global mordax_thread_join
.type mordax_thread_join, %function
mordax_thread_join:
	svc #MORDAX_SYSCALL_THREAD_JOIN
	bx lr

@ Retrieves information about the current thread and/or process.
@ Parameters:
@ 	r0 = code selecting requested information
@ Return values:
@ 	r0 = the requested information
.global mordax_thread_info
.type mordax_thread_info, %function
mordax_thread_info:
	svc #MORDAX_SYSCALL_THREAD_INFO
	bx lr

@ Creates a new process.
@ Parameters:
@ 	r0 = pointer to a process information structure
@ Return values:
@ 	r0 = the PID of the new process or -1 on error
.global mordax_process_create
.type mordax_process_create, %function
mordax_process_create:
	svc #MORDAX_SYSCALL_PROCESS_CREATE
	bx lr

