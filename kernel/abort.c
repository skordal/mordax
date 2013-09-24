// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "abort.h"
#include "debug.h"

bool abort_handler(struct thread_context * context)
{
	struct abort_details details;
	abort_get_details(&details, context);

	debug_printf("Invalid memory %s access at %p\n",
		details.type == ABORT_READ ? "read" : "write",
		details.address);

	return false;
}

