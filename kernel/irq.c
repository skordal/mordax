// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 - 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "irq.h"

#include "debug.h"
#include "drivers/interrupts/intc.h"

static struct intc_driver * driver = 0;

// IRQ handler function, called from the assembly interrupt handler:
void irq_interrupt_handler(struct thread_context * context)
{
	driver->handle_irq(context);
}

void irq_set_intc_driver(struct intc_driver * d)
{
	driver = d;
}

void irq_register(unsigned irq, irq_handler_func handler, void * data_ptr)
{
	driver->register_handler(irq, handler, data_ptr);
}

void irq_unregister(unsigned irq)
{
	driver->unregister_handler(irq);
}

void irq_enable(unsigned irq)
{
	driver->enable_irq(irq);
}

void irq_disable(unsigned irq)
{
	driver->disable_irq(irq);
}

irq_handler_func irq_get_handler(unsigned irq)
{
	return driver->get_handler(irq);
}

