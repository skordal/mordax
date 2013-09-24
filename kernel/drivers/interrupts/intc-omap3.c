// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "intc-omap3.h"

#include <debug.h>
#include <mmu.h>

static volatile uint32_t * memory;

static irq_handler_func irq_handlers[96];

bool intc_omap3_initialize(struct dt_node * device_node)
{
	uint32_t memory_info[2];
	if(!dt_get_array32_property(device_node, "reg", memory_info, 2))
	{
		debug_printf("Error: interrupt controller node \"%s\" has no \"reg\" property!\n",
			device_node->name);
		return false;
	}

	memory = mmu_map_device((physical_ptr) memory_info[0], (size_t) memory_info[1]);

	// Reset the interrupt controller:
	memory[INTC_OMAP3_SYSCONFIG] |= 1 << INTC_OMAP3_SYSCONFIG_SOFTRESET;

	// While resetting, zero the irq handler array:
	for(int i = 0; i < 96; ++i)
		irq_handlers[i] = 0;

	// Wait until the reset is complete:
	while(!(memory[INTC_OMAP3_SYSSTATUS] & (1 << INTC_OMAP3_SYSSTATUS_RESETDONE)));

	// Set up power management:
	memory[INTC_OMAP3_SYSCONFIG] = (1 << INTC_OMAP3_SYSCONFIG_AUTOIDLE);
	memory[INTC_OMAP3_IDLE] = (1 << INTC_OMAP3_IDLE_TURBO);

	// Enable the protection feature:
	memory[INTC_OMAP3_PROTECTION] = (1 << INTC_OMAP3_PROTECTION_PROTECTION);

	return true;
}

void intc_omap3_handle_irq(void)
{
	// Get the number of the IRQ to handle:
	unsigned active_irq = memory[INTC_OMAP3_SIR_IRQ] & INTC_OMAP3_SIR_IRQ_ACTIVEIRQ_MASK;
	debug_printf("Active IRQ: %d\n", active_irq);

	// Call the IRQ handler:
	irq_handlers[active_irq](active_irq);

	// Reset interrupt generation:
	memory[INTC_OMAP3_CONTROL] |= (1 << INTC_OMAP3_CONTROL_NEWIRQAGR);
	asm volatile("dsb\n\t");
}

void intc_omap3_register_handler(unsigned irq, irq_handler_func handler)
{
	if(irq >= 96)
		return;
	irq_handlers[irq] = handler;

	// Enable the specified IRQ:
	int mir_register = irq >> 6;	
	memory[INTC_OMAP3_MIR_SET(mir_register)] = (1 << (irq % 32));
}

void intc_omap3_unregister_handler(unsigned irq)
{
	if(irq >= 96)
		return;
	irq_handlers[irq] = 0;

	// Disable the specified IRQ:
	int mir_register = irq >> 6;	
	memory[INTC_OMAP3_MIR_CLEAR(mir_register)] = (1 << (irq % 32));
}

