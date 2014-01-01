// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "registers.h"

#include "../debug.h"
#include "../mm.h"
#include "../mmu.h"

// The stacks are lain out as follows: the supervisor mode stack is mapped to
// address 0x00000000. The other stacks are mapped one page below the area
// occupied by the previous stack. This makes sure that any operation that
// causes the stack to be depleted causes an invalid memory access so that
// the problem can be discovered.

// Allocates and sets up the stacks neccessary for supervisor, abort,
// undefined, irq and fiq modes:
void stacks_initialize(void)
{
	uint32_t stack_address;
	struct mm_physical_memory stack_memory;

	// Allocate and set up the supervisor stack:
	stack_address = 0x00000000;
	mm_allocate_physical(CONFIG_SYSTEM_STACK_SIZE, &stack_memory);
	mmu_map(0, stack_memory.base, (void *) (stack_address - CONFIG_SYSTEM_STACK_SIZE),
		CONFIG_SYSTEM_STACK_SIZE, MORDAX_TYPE_STACK, MORDAX_PERM_RW_NA);
	asm volatile(
		"cps %[svc_mode]\n\t"
		"mov sp, %[stack_address]\n\t"
		"cps %[sys_mode]\n\t"
		:: [svc_mode] "I" (PROCESSOR_MODE_SVC), [sys_mode] "I" (PROCESSOR_MODE_SYS),
		[stack_address] "r" (stack_address));

	// Allocate and set up the abort stack:
	stack_address -= CONFIG_SYSTEM_STACK_SIZE - CONFIG_PAGE_SIZE;
	mm_allocate_physical(CONFIG_SYSTEM_STACK_SIZE, &stack_memory);
	mmu_map(0, stack_memory.base, (void *) (stack_address - CONFIG_SYSTEM_STACK_SIZE),
		CONFIG_SYSTEM_STACK_SIZE, MORDAX_TYPE_STACK, MORDAX_PERM_RW_NA);
	asm volatile(
		"cps %[abt_mode]\n\t"
		"mov sp, %[stack_address]\n\t"
		"cps %[sys_mode]\n\t"
		:: [abt_mode] "I" (PROCESSOR_MODE_ABT), [sys_mode] "I" (PROCESSOR_MODE_SYS),
		[stack_address] "r" (stack_address));

	// Allocate and set up the undefined instruction mode stack:
	stack_address -= CONFIG_SYSTEM_STACK_SIZE - CONFIG_PAGE_SIZE;
	mm_allocate_physical(CONFIG_SYSTEM_STACK_SIZE, &stack_memory);
	mmu_map(0, stack_memory.base, (void *) (stack_address - CONFIG_SYSTEM_STACK_SIZE),
		CONFIG_SYSTEM_STACK_SIZE, MORDAX_TYPE_STACK, MORDAX_PERM_RW_NA);
	asm volatile(
		"cps %[und_mode]\n\t"
		"mov sp, %[stack_address]\n\t"
		"cps %[sys_mode]\n\t"
		:: [und_mode] "I" (PROCESSOR_MODE_UND), [sys_mode] "I" (PROCESSOR_MODE_SYS),
		[stack_address] "r" (stack_address));

	// Allocate and set up the IRQ mode stack:
	stack_address -= CONFIG_SYSTEM_STACK_SIZE - CONFIG_PAGE_SIZE;
	mm_allocate_physical(CONFIG_SYSTEM_STACK_SIZE, &stack_memory);
	mmu_map(0, stack_memory.base, (void *) (stack_address - CONFIG_SYSTEM_STACK_SIZE),
		CONFIG_SYSTEM_STACK_SIZE, MORDAX_TYPE_STACK, MORDAX_PERM_RW_NA);
	asm volatile(
		"cps %[irq_mode]\n\t"
		"mov sp, %[stack_address]\n\t"
		"cps %[sys_mode]\n\t"
		:: [irq_mode] "I" (PROCESSOR_MODE_IRQ), [sys_mode] "I" (PROCESSOR_MODE_SYS),
		[stack_address] "r" (stack_address));

	// Allocate and set up the FIQ mode stack:
	stack_address -= CONFIG_SYSTEM_STACK_SIZE - CONFIG_PAGE_SIZE;
	mm_allocate_physical(CONFIG_SYSTEM_STACK_SIZE, &stack_memory);
	mmu_map(0, stack_memory.base, (void *) (stack_address - CONFIG_SYSTEM_STACK_SIZE),
		CONFIG_SYSTEM_STACK_SIZE, MORDAX_TYPE_STACK, MORDAX_PERM_RW_NA);
	asm volatile(
		"cps %[fiq_mode]\n\t"
		"mov sp, %[stack_address]\n\t"
		"cps %[sys_mode]\n\t"
		:: [fiq_mode] "I" (PROCESSOR_MODE_FIQ), [sys_mode] "I" (PROCESSOR_MODE_SYS),
		[stack_address] "r" (stack_address));
}

