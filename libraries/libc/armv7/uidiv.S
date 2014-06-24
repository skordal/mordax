@ The Mordax Standard Library
@ (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

.section .text

@ Unsigned integer division, required by GCC. Calculates r0 / r1.
@ Arguments:
@ 	r0 - Numerator
@	r1 - Denominator
.global __aeabi_uidiv
.type __aeabi_uidiv, %function
__aeabi_uidiv:
	mov r3, #0
1:
	cmp r0, r1
	addeq r3, #1
	moveq r0, r3
	moveq pc, lr

	addhi r3, #1
	subhi r0, r1
	bhi 1b

	mov r0, r3
	mov pc, lr

