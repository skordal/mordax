// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_IRQ_H
#define MORDAX_IRQ_H

struct thread_context;

/** IRQ handler function. */
typedef void (*irq_handler_func)(struct thread_context * context, unsigned irq);

// Driver structure forward-declaration:
struct intc_driver;

/**
 * Sets the driver to use for the interrupt controller.
 * @param driver the interrupt controller driver to use.
 */
void irq_set_intc_driver(struct intc_driver * driver);

/**
 * Registers an IRQ handler.
 * @param irq the number of the irq to handle.
 * @param handler pointer to the IRQ handling function.
 */
void irq_register(unsigned irq, irq_handler_func handler);

/**
 * Unregisters an IRQ handler.
 * @param irq the number for the IRQ which handler to remove.
 */
void irq_unregister(unsigned irq);

/**
 * Gets an IRQ handler.
 * @param irq the number of the IRQ to return the handler for.
 * @return a pointer to the active handler function or 0 if no
 *         handler is registered.
 */
irq_handler_func irq_get_handler(unsigned irq);

#endif

