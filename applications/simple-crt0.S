# The Mordax Microkernel OS Multithreading Test Programme
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

@ Simple initialization code for simple test applications.

.section .init

.global _start
.type _start, %function
_start:
	mov r0, #0
	ldr r1, =start_message
	bl mordax_system

	bl main
	b mordax_thread_exit
1:
	wfi
	b 1b

.balign 4
start_message:
	.asciz "simple-crt0.o started"

