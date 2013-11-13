// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "mm.h"
#include "process.h"
#include "scheduler.h"
#include "thread.h"

struct thread * thread_create(struct process * parent, void * entrypoint, void * stack)
{
	struct thread * retval = mm_allocate(sizeof(struct thread), MM_DEFAULT_ALIGNMENT,
		MM_MEM_NORMAL);
	retval->parent = parent;
	retval->tid = -1;
	retval->context = context_new();
	retval->exit_listeners = queue_new();

	context_set_pc(retval->context, entrypoint);
	context_set_sp(retval->context, stack);

	debug_printf("Creating new thread for process %p:\n", parent);
	debug_printf("\tEntry: %p\n", entrypoint);
	debug_printf("\tStack: %p\n", stack);

	return retval;
}

void thread_free(struct thread * t, int retval)
{
	// Iterate through all waiting threads and release them from
	// the blocking queue:
	struct queue_node * current = t->exit_listeners->first;
	while(current != 0)
	{
		struct thread * listener = current->data;
		context_set_syscall_retval(listener->context, (void *) retval);
		scheduler_move_thread_to_running(listener);

		current = current->next;
	}

	struct process * parent = t->parent;
	if(parent != 0)
		process_remove_thread(t->parent, t);

	// Free the thread structure and its members:
	context_free(t->context);
	mm_free(t);
}

void thread_add_exit_listener(struct thread * t, struct thread * l)
{
	queue_add_front(t->exit_listeners, l);
}

