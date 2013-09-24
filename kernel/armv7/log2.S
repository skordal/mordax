@ The Mordax Microkernel
@ (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

.section .text

@ Gets the base-2 logarithm of the specified unsigned integer.
@ Arguments:
@	r0 - input number
.global log2
.type log2, %function
log2:
	mov r1, #31
	clz r0, r0
	sub r0, r1, r0
	mov pc, lr

