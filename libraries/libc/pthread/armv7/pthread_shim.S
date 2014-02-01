@ The Mordax Standard Library
@ (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

@ POSIX threads shim - this is the actual entrypoint for all newly created
@ threads. This function makes sure that the thread is correctly terminated
@ when the entry-point function returns, and it also makes sure that the
@ correct argument is passed to the entry-point funciton.
@
@ If pthread_exit is called from the thread, the thread is terminated without
@ returning control to this function.
@
@ Upon entry to this function, the stack contains the following:
@ 	(highest address)
@	* entry-point function address
@	* argument pointer
@	(lowest address)
@ The stack pointer is set at the top of the stack.
.global __pthread_shim
.type __pthread_shim, %function
__pthread_shim:
	ldmdb sp, {r0, r1}	@ Load entry-point function address and argument
	blx r1			@ Start entry-point function
	b __pthread_terminate	@ Terminate the thread and store the return value

