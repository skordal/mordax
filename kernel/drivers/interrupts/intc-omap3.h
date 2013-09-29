// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_DRIVERS_INTERRUPTS_INTC_OMAP3_H
#define MORDAX_DRIVERS_INTERRUPTS_INTC_OMAP3_H

#include <dt.h>
#include <irq.h>
#include <types.h>

// Register indices:
#define INTC_OMAP3_REVISION	(0x00 >> 2)
#define INTC_OMAP3_SYSCONFIG	(0x10 >> 2)
#define INTC_OMAP3_SYSSTATUS	(0x14 >> 2)
#define INTC_OMAP3_SIR_IRQ	(0x40 >> 2)
#define INTC_OMAP3_CONTROL	(0x48 >> 2)
#define INTC_OMAP3_PROTECTION	(0x4c >> 2)
#define INTC_OMAP3_IDLE		(0x50 >> 2)

#define INTC_OMAP3_MIR_CLEAR(n)	((0x088 + (n * 0x20)) >> 2)
#define INTC_OMAP3_MIR_SET(n)	((0x08c + (n * 0x20)) >> 2)
#define INTC_OMAP3_ILR(n)	((0x100 + (n *  0x4)) >> 2)

#define INTC_OMAP3_SIR_IRQ_ACTIVEIRQ_MASK	0x7f

// SYSCONFIG bit names:
#define INTC_OMAP3_SYSCONFIG_AUTOIDLE		0
#define INTC_OMAP3_SYSCONFIG_SOFTRESET		1

// SYSSTATUS bit names:
#define INTC_OMAP3_SYSSTATUS_RESETDONE		0

// CONTROL bit names:
#define INTC_OMAP3_CONTROL_NEWIRQAGR		0

// PROTECTION bit names:
#define INTC_OMAP3_PROTECTION_PROTECTION	0

// IDLE bit names:
#define INTC_OMAP3_IDLE_TURBO			1

bool intc_omap3_initialize(struct dt_node * device_node);
void intc_omap3_handle_irq(struct thread_context * context);
void intc_omap3_register_handler(unsigned irq, irq_handler_func handler);
void intc_omap3_unregister_handler(unsigned irq);

#endif

