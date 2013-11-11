// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "mm.h"
#include "process.h"
#include "thread.h"

struct thread * thread_create(struct process * parent, void * entrypoint, void * stack)
{
	struct thread * retval = mm_allocate(sizeof(struct thread), MM_DEFAULT_ALIGNMENT,
		MM_MEM_NORMAL);
	retval->parent = parent;
	retval->tid = process_allocate_tid(parent);
	retval->context = context_new();

	context_set_pc(retval->context, entrypoint);
	context_set_sp(retval->context, stack);

	debug_printf("Creating new thread for process %p:\n", parent);
	debug_printf("\tEntry: %p\n", entrypoint);
	debug_printf("\tStack: %p\n", stack);

	return retval;
}

void thread_free(struct thread * t)
{
	struct process * parent = t->parent;
	process_remove_thread(t->parent, t);
	process_free_tid(parent, t->tid);

	// Free the thread structure and its members:
	context_free(t->context);
	mm_free(t);
}

