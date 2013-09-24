@ The Mordax Microkernel
@ (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

.section .text

@ Copies memory from the source to the destination.
@ Arguments:
@	r0 - destination
@	r1 - source
@	r2 - length
.global memcpy
.type memcpy, %function
memcpy:
	cmp r2, #0	@ Test for length == 0
	moveq pc, lr

	tst r2, #3	@ Test for length % 4 == 0
	beq memcpy_4

1:
	ldrb r3, [r1], #1
	strb r3, [r0], #1
	subs r2, #1
	bne 1b

	mov pc, lr

@ Copies memory with a length that is a multiple of 4; arguments
@ are the same as for memcpy.
memcpy_4:
1:
	ldr r3, [r1], #4
	str r3, [r0], #4
	subs r2, #4
	bne 1b

	mov pc, lr

