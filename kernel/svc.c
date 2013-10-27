// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "context.h"
#include "debug.h"
#include "svc.h"

// Supervisor call (software interrupt) handler, called by target assembly
// code:
void svc_interrupt_handler(struct thread_context * context, uint8_t svc)
{
	debug_printf("Supervisor call: %d, context at %p\n", svc, context);
	debug_printf("\tArgument 1: %x\n", context_get_svc_argument(context, 0));
	debug_prinff("\tArgument 2: %x\n", context_get_svc_argument(context, 1));
	debug_printf("\tArgument 3: %x\n", context_get_svc_argument(context, 2));
	debug_printf("\tReturning 0...\n");
	context_set_svc_retval(context, 0);
}

