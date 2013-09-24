// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "irq.h"

#include "debug.h"
#include "drivers/interrupts/intc.h"

static struct intc_driver * driver = 0;

// IRQ handler function, called from the assembly interrupt handler:
void irq_interrupt_handler(void)
{
	debug_printf("IRQ received!\n");
	driver->handle_irq();
}

void irq_set_intc_driver(struct intc_driver * d)
{
	driver = d;
}

void irq_register(unsigned irq, irq_handler_func handler)
{
	driver->register_handler(irq, handler);
}

void irq_unregister(unsigned irq)
{
	driver->unregister_handler(irq);
}


