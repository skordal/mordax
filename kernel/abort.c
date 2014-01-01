// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "abort.h"
#include "context.h"
#include "debug.h"
#include "kernel.h"

bool abort_handler(struct thread_context * context)
{
	struct abort_details details;
	abort_get_details(&details, context);

	if(details.mode == ABORT_KERNEL)
	{
		debug_printf("\nINVALID MEMORY ACCESS IN KERNEL MODE\n");
		debug_printf("Cannot %s address %x\n",
			details.type == ABORT_READ ? "read from" : "write to",
			details.address);
		context_print(context);
		kernel_panic("unrecoverable memory access error in kernel mode");
	} else {
		debug_printf("Invalid memory %s access at %p\n",
			details.type == ABORT_READ ? "read" : "write",
			details.address);
		context_print(context);
		kernel_panic("unrecoverable memory access error in user mode");
	}

	return false;
}

