// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "mm.h"
#include "scheduler.h"
#include "thread.h"

struct thread * thread_create(struct process * parent, void * entrypoint, void * stack)
{
	struct thread * retval = mm_allocate(sizeof(struct thread), MM_DEFAULT_ALIGNMENT,
		MM_MEM_NORMAL);
	retval->parent = parent;
	retval->pid = scheduler_allocate_pid();
	retval->context = context_new();

	context_set_pc(retval->context, entrypoint);
	context_set_sp(retval->context, stack);

	debug_printf("Creating new thread for process %p:\n", parent);
	debug_printf("\tEntry: %p\n", entrypoint);
	debug_printf("\tStack: %p\n", stack);

	return retval;
}


