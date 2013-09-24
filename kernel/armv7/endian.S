@ The Mordax Microkernel
@ (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

#ifndef CONFIG_LITTLE_ENDIAN
#error "Big endian configurations are currently not supported."
#endif

.section .text

@ Converts a 32-bit big endian integer to a 32-bit little endian integer.
@ Arguments:
@	r0 - big endian input
.global be2le32
.type be2le32, %function
be2le32:
	rev r0, r0
	mov pc, lr

@ Converts a 64-bit big endian integer to a 64-bit little endian integer.
@ Arguments:
@	r0 - big endian input low bits.
@	r1 - big endian input high bits.
.global be2le64
.type be2le64, %function
be2le64:
	rev r2, r0
	rev r0, r1
	mov r1, r2

	mov pc, lr

