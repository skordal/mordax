// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_DRIVERS_INTERRUPTS_INTC_H
#define MORDAX_DRIVERS_INTERRUPTS_INTC_H

#include <dt.h>
#include <irq.h>
#include <types.h>

/** Function type for initializing an interrupt controller. */
typedef bool (*intc_driver_init_func)(struct dt_node * device_node);

/** Function type for handling an IRQ interrupt. */
typedef void (*intc_driver_handle_func)(struct thread_context * context);

/** Function type for registering an IRQ handler. */
typedef void (*intc_driver_register_func)(unsigned irq, irq_handler_func handler);

/** Function type for removing a registered IRQ handler. */
typedef void (*intc_driver_unregister_func)(unsigned irq);

/**
 * Interrupt controller driver structure.
 * @todo Support interrupt priorities.
 */
struct intc_driver
{
	intc_driver_init_func initialize;
	intc_driver_handle_func handle_irq;
	intc_driver_register_func register_handler;
	intc_driver_unregister_func unregister_handler;
};

/**
 * Instantiates an interrupt controller driver based on the specified device node.
 * @param device_node device node containing information about the desired interrupt controller.
 * @return an instance of the device driver for the interrupt controller.
 */
struct intc_driver * intc_driver_instantiate(struct dt_node * device_node);

#endif

