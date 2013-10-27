// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "context.h"
#include "syscall.h"

// System call handler, called by target assembly code:
void syscall_interrupt_handler(struct thread_context * context, uint8_t svc)
{
	debug_printf("Supervisor call: %d, context at %p\n", svc, context);
	debug_printf("\tArgument 1: %x\n", context_get_syscall_argument(context, 0));
	debug_printf("\tArgument 2: %x\n", context_get_syscall_argument(context, 1));
	debug_printf("\tArgument 3: %x\n", context_get_syscall_argument(context, 2));
	debug_printf("\tReturning 0...\n");
	context_set_syscall_retval(context, 0);
}

