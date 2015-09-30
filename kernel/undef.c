// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "context.h"
#include "kernel.h"
#include "undef.h"

void undef_interrupt_handler(struct thread_context * context)
{
	context_print(context);
	kernel_panic("unknown instruction attempted");
}

