// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 - 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <debug.h>
#include <dt.h>
#include <irq.h>
#include <mmu.h>

#include "timer.h"
#include "timer-omap3.h"

static volatile uint32_t * memory;
static unsigned int interval;
static timer_callback_func callback;

// IRQ handler for the timer driver; the interrupt number is retrieved
// from the device tree when initializing.
static void timer_omap3_irq_handler(struct thread_context * context, unsigned irq, void * data_ptr);

bool timer_omap3_initialize(struct dt_node * device_node)
{
	uint32_t memory_info[2];
	if(!dt_get_array32_property(device_node, "reg", memory_info, 2))
	{
		debug_printf("Error: timer node \"%s\" has no \"reg\" property\n",
			device_node->name);
		return false;
	}
	memory = mmu_map_device((physical_ptr) memory_info[0], (size_t) memory_info[1]);

	// Reset the module:
	memory[TIMER_OMAP3_TIOCP_CFG] = 1 << TIMER_OMAP3_SOFTRESET;
	while(!(memory[TIMER_OMAP3_TISTAT] & (1 << TIMER_OMAP3_RESETDONE)));

	// Set up power management:
	memory[TIMER_OMAP3_TIOCP_CFG] = 1 << TIMER_OMAP3_AUTOIDLE | 2 << TIMER_OMAP3_IDLEMODE;

	// Register the IRQ handler:
	uint32_t irq_number;
	if(!dt_get_array32_property(device_node, "interrupts", &irq_number, 1))
	{
		debug_printf("Error: timer noe \"%s\" has no \"interrupts\" property\n",
			device_node->name);
	}

	// Register and enable the interrupt handler:
	irq_register(irq_number, timer_omap3_irq_handler, 0);
	irq_enable(irq_number);

	return true;
}

void timer_omap3_set_interval(unsigned int new_interval)
{
	interval = new_interval * (TIMER_OMAP3_FCLK / 1000000);
}

void timer_omap3_set_callback(timer_callback_func new_callback)
{
	callback = new_callback;
}

void timer_omap3_start(void)
{
	// Set up the interval value:
	memory[TIMER_OMAP3_TLDR] = -interval;
	memory[TIMER_OMAP3_TTGR] = 1; // triggers a counter reload

	// Set the timer mode to autoreload the counter value on overflow:
	memory[TIMER_OMAP3_TCLR] = 1 << TIMER_OMAP3_TRG | 1 << TIMER_OMAP3_AR;
	// Enable the overflow interrupt:
	memory[TIMER_OMAP3_TIER] = 1 << TIMER_OMAP3_OVF_IT_ENA;

	// Starts the timer:
	memory[TIMER_OMAP3_TCLR] |= 1 << TIMER_OMAP3_ST;
}

void timer_omap3_stop(void)
{
	memory[TIMER_OMAP3_TCLR] &= ~(1 << TIMER_OMAP3_ST);
}

static void timer_omap3_irq_handler(struct thread_context * context, unsigned irq, void * data_ptr)
{
	if(callback != 0)
		callback();
	memory[TIMER_OMAP3_TISR] = 0x7;
}

