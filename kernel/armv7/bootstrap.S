@ The Mordax Microkernel
@ (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

#include "mmu.h"
#include "registers.h"

#ifndef CONFIG_INITIAL_HEAP_SIZE
#error "initial heap size not set, use CONFIG_INITIAL_HEAP_SIZE with a proper value"
#endif

.section .init

@ Exception vectors, placed at the beginning of the executable:
.global exception_vectors
exception_vectors:
	ldr pc, =bootstrap_entry
	ldr pc, =int_undefined_instruction
	ldr pc, =int_software_interrupt
	ldr pc, =int_prefetch_abort
	ldr pc, =int_data_abort
	movs pc, lr			@ Unused interrupt
	ldr pc, =int_irq
	ldr pc, =int_fiq

.ltorg

@ Kernel entrypoint; u-boot passes control here after loading the kernel.
@ On entry, the first tree registers should contain the following information:
@	r0: 0
@	r1: machine type number
@	r2: pointer to the physical address of the device tree blob.
.global bootstrap_entry
.type bootstrap_entry, %function
bootstrap_entry:
	cpsid aif, #PROCESSOR_MODE_SYS	@ Switch to system mode
	ldr sp, =load_address		@ Set up a temporary stack

	@ Set up the arguments for the kernel_main call:
	mov r0, r2		@ Move the device tree pointer to r0
	ldr r1, [r0, #4]	@ Load the size of the device tree into r1
	rev r1, r1

	@ Calculate the offset to subtract from virtual addresses to get the
	@ physical 
	ldr r3, =kernel_address
	ldr r4, =load_address
	sub r3, r3, r4

bootstrap_clear_bss:
	ldr r4, =bss_start
	ldr r5, =bss_end
	sub r4, r4, r3
	sub r5, r5, r3

	mov r6, #0
1:
	cmp r4, r5
	strne r6, [r4], #4
	bne 1b

bootstrap_init_mmu:
	ldr r4, = \
		  NMRR_WT_NWA << NMRR_IR(0) | NMRR_WT_NWA << NMRR_OR(0) \
		| NMRR_WT_NWA << NMRR_IR(1) | NMRR_WT_NWA << NMRR_OR(1) \
		| NMRR_WB_WA  << NMRR_IR(2) | NMRR_WB_WA  << NMRR_OR(2) \
		| NMRR_WB_WA  << NMRR_IR(3) | NMRR_WB_WA  << NMRR_OR(3) \
		| NMRR_NC     << NMRR_IR(4) | NMRR_NC     << NMRR_OR(4) \
		| NMRR_NC     << NMRR_IR(5) | NMRR_NC     << NMRR_OR(5) \
		/* 6 is reserved */ \
		| NMRR_NC     << NMRR_IR(7) | NMRR_NC     << NMRR_OR(7)
	mcr p15, 0, r4, c10, c2, 1	@ Write the NMRR register

	ldr r4, = \
		  PRRR_NS1 | PRRR_DS1 \
		| PRRR_TR_NORM << PRRR_TR(0) \
		| PRRR_TR_NORM << PRRR_TR(1) \
		| PRRR_TR_NORM << PRRR_TR(2) \
		| PRRR_TR_NORM << PRRR_TR(3) \
		| PRRR_TR_SO   << PRRR_TR(4) \
		| PRRR_TR_DEV  << PRRR_TR(5) \
		/* 6 is reserved */ \
		| PRRR_TR_NORM << PRRR_TR(7)
	mcr p15, 0, r4, c10, c2, 0	@ Write the PRRR register

	@ Calculate the end of the mapped memory area:
	ldr r7, =bss_end
	ldr r8, =1024 * 1024 - 1
	add r8, r8, r7
	add r8, r8, #CONFIG_INITIAL_HEAP_SIZE	@ Allocate the initial heap.
	ldr r9, =0xfff00000
	and r8, r9, r8

	@ Store the new kernel dataspace end pointer:
	ldr r9, =kernel_dataspace_end
	sub r9, r9, r3
	str r8, [r9]

	ldr r4, =load_address
	ldr r5, =kernel_address
	ldr r6, =kernel_translation_table
	sub r6, r6, r3

	@ Use 1 Mb executable section mappings for the initial MMU mappings.
	@ldr r8, =MMU_SECTION_TYPE | MMU_SECTION_UNCACHED | MMU_SECTION_RW_NA
	ldr r8, =MMU_SECTION_TYPE | MMU_SECTION_CODE | MMU_SECTION_RW_NA

1:
	cmp r5, r7
	orrlo r9, r8, r4
	strlo r9, [r6, r4, lsr #18]
	strlo r9, [r6, r5, lsr #18]
	addlo r4, r4, #1024 * 1024
	addlo r5, r5, #1024 * 1024
	blo 1b	

#ifdef CONFIG_EARLY_DEBUG
	@ Create a mapping for the UART, for testing:
	ldr r4, =(CONFIG_EARLY_DEBUG_ADDRESS & -(1024*1024)) \
		| MMU_SECTION_TYPE | MMU_SECTION_DEVICE | MMU_SECTION_RW_NA | 1 << MMU_SECTION_XN
	str r4, [r6, r4, lsr #18]
#endif

	mcr p15, 0, r2, c8, c7, 0	@ Clear the TLB
	mcr p15, 0, r6, c2, c0, 0	@ Set TTBR0
	mcr p15, 0, r6, c2, c0, 0	@ Set TTBR1
	mvn r4, #0
	mcr p15, 0, r4, c3, c0, 0	@ Set domain permissions (full access)
	mcr p15, 0, r2, c8, c7, 0	@ Clear the TLB

	@ Enable the MMU, tex remapping, branch prediction and caches:
	ldr r2, =1 << SCTLR_M | 1 << SCTLR_TRE | 1 << SCTLR_Z | 1 << SCTLR_I | 1 << SCTLR_C
	mcr p15, 0, r2, c1, c0, 0

	isb
	dsb

	ldr pc, =bootstrap_finished

.ltorg

.section .text
bootstrap_finished:
	clrex

	@ FIXME: Temporarily use the end of the mapped dataspace as stack.
	ldr sp, =kernel_dataspace_end
	ldr sp, [sp]

	bl kernel_main

loopy:
	wfi
	b loopy

