// The Mordax Microkernel
// (c) Kristian Klomsten Skordal <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "context.h"
#include "lock.h"
#include "mm.h"
#include "queue.h"
#include "scheduler.h"

#include "api/errno.h"

struct lock
{
	struct thread * aquired;
	struct queue * waiting;
};

// Function used to release all waiting threads when destroying a lock:
static void lock_release_waiting(struct thread * t);

struct lock * lock_create(void)
{
	struct lock * retval = mm_allocate(sizeof(struct lock), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	retval->aquired = 0;
	retval->waiting = queue_new();
	return retval;
}

void lock_destroy(struct lock * l)
{
	queue_free(l->waiting, (queue_data_free_func) lock_release_waiting);
	mm_free(l);
}

int lock_aquire(struct lock * l, struct thread * t, bool * blocking)
{
	*blocking = false;
	if(l->aquired == t)
		return -EDEADLK;

	if(l->aquired == 0)
		l->aquired = t;
	else {
		*blocking = true;
		queue_add_back(l->waiting, t);
		scheduler_move_thread_to_blocking(t);
	}

	return 0;
}

int lock_release(struct lock * l, struct thread * t)
{
	if(l->aquired != t)
		return -EINVAL;

	struct thread * waiting_thread = 0;
	if(queue_remove_front(l->waiting, (void **) &waiting_thread))
	{
		l->aquired = waiting_thread;
		context_set_syscall_retval(waiting_thread->context, 0);
		scheduler_move_thread_to_running(waiting_thread);
	} else
		l->aquired = 0;

	return 0;
}

static void lock_release_waiting(struct thread * t)
{
	context_set_syscall_retval(t->context, (void *) -EIDRM);
	scheduler_move_thread_to_running(t);
}

