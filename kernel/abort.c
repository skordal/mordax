// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "abort.h"
#include "debug.h"

void abort_handler(struct thread_context * process_block)
{
	debug_printf("Data abort with context @ %p\n", process_block);
}

