@ The Mordax Microkernel
@ (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

#ifdef CONFIG_EARLY_DEBUG

#ifndef CONFIG_EARLY_DEBUG_ADDRESS
#error "No address has been set for EARLY_DEBUG_ADDRESS!"
#endif

.section .text

@ Early debug output function. Prints a character to the 16c750 UART at the
@ address specified by EARLY_DEBUG_ADDRESS.
@ Arguments:
@ 	r0 - character to print.
.global early_putc
.type early_putc, %function
early_putc:
	ldr r1, =CONFIG_EARLY_DEBUG_ADDRESS

1:
	ldrb r2, [r1, #0x14]	@ Read the LSR register
	tst r2, #1 << 5		@ Check the TX_FIFO_E bit
	beq 1b			@ Loop until the bit is set.

	strb r0, [r1]		@ Send the character
	mov pc, lr

#endif

