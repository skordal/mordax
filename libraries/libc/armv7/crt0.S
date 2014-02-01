@ The Mordax Standard Library
@ (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

#include <mordax/system.h>

.section .init

@ Application main entry point.
@ This function assumes that two arguments are located at the stack; argc and
@ argv.
.global _start
_start:
	bl __initialize

	@ The the address of the top of the stack:
	mov r0, #MORDAX_SYSTEM_GETSPLIT
	bl mordax_system

	cmp sp, r0

	@ Load argc and argv from the stack unless the stack pointer points to the absolute top of the stack:
	ittee ne
	ldrne r0, [sp, #-8] @ Load argc
	ldrne r1, [sp, #-4] @ Load argv
	moveq r0, #0
	moveq r0, r1

	@ Start main:
	bl main

	b mordax_thread_exit
