@ The Mordax Microkernel
@ (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

#include "registers.h"

.section .text

.global interrupts_initialize
interrupts_initialize:
	ldr r0, =kernel_address
	mcr p15, 0, r0, c12, c0, 0
	cpsie if
	mov pc, lr
.ltorg

@ Stores the current execution context, to allow for context switching
@ in interrupt handlers:
.macro store_context
	sub sp, #15 * 4
	stmia sp, {r0 - r14}^	@ Store user-mode registers
	mrs r0, spsr
	stmdb sp!, {r0, lr}	@ Exception return address and SPSR
.endm

@ Restores the current execution context:
.macro restore_context
	ldmia sp!, {r0, lr}	@ Exception return address and SPSR
	msr spsr, r0
	ldmia sp, {r0 - r14}^	@ Load user-mode registers
	add sp, #15 * 4
.endm

@ Undefined instruction exception handler.
.global int_undefined_instruction
.type int_undefined_instruction, %function
int_undefined_instruction:
	store_context

	restore_context
	movs pc, lr

@ Software interrupt exception handler.
.global int_software_interrupt
.type int_software_interrupt, %function
int_software_interrupt:
	store_context

	mov r0, sp
	bl abort_handler

	restore_context
	movs pc, lr

@ Prefetch abort exception handler.
.global int_prefetch_abort
.type int_prefetch_abort, %function
int_prefetch_abort:
	store_context

	mov r0, sp
	bl abort_handler

	restore_context
	subs pc, lr, #4

@ Data abort exception handler.
.global int_data_abort
.type int_data_abort, %function
int_data_abort:
	store_context

	mov r0, sp
	bl abort_handler

	restore_context
	subs pc, lr, #4

@ IRQ exception handler.
.global int_irq
.type int_irq, %function
int_irq:
	store_context

	@ Pass a pointer to the stored context data to the handler function:
	mov r0, sp

	push {lr}
	bl irq_interrupt_handler
	pop {lr}

	restore_context
	subs pc, lr, #4

@ FIQ exception handler.
.global int_fiq
.type int_fiq, %function
int_fiq:
	subs pc, lr, #4

