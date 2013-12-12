// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "context.h"
#include "debug.h"
#include "process.h"
#include "scheduler.h"
#include "syscall.h"
#include "thread.h"

#include "api/syscalls.h"
#include "api/thread.h"

// System call handler, called by target assembly code:
void syscall_interrupt_handler(struct thread_context * context, uint8_t syscall)
{
	debug_printf("System call from process %d thread %d: %d\n",
		active_thread->parent->pid, active_thread->tid, syscall);

	// TODO: replace this switch with an array or something else more clever
	switch(syscall)
	{
		case MORDAX_SYSCALL_SYSTEM:
			syscall_system(context);
			break;
		case MORDAX_SYSCALL_THREAD_EXIT:
			syscall_thread_exit(context);
			break;
		case MORDAX_SYSCALL_THREAD_CREATE:
			syscall_thread_create(context);
			break;
		case MORDAX_SYSCALL_THREAD_JOIN:
			syscall_thread_join(context);
			break;
		default: // TODO: handle unrecognized system calls
			debug_printf("Unknown system call %d\n", syscall);
			break;
	}
}

void syscall_system(struct thread_context * context)
{
	int function = context_get_syscall_argument(context, 0);

	switch(function)
	{
		case 0:
		{
			const char * string = context_get_syscall_argument(context, 1);
			debug_printf("[SYSCALL0: DEBUG] PID %d, TID %d: %s\n", active_thread->parent->pid,
				active_thread->tid, string);
			break;
		}
		default:
			debug_printf("Unknown function for the system syscall: %d\n", function);
			break;
	}
}

void syscall_thread_exit(struct thread_context * context)
{
	struct thread * removed_thread = scheduler_remove_thread(active_thread);
	thread_free(removed_thread, (int) context_get_syscall_argument(context, 0));
	scheduler_reschedule();
}

void syscall_thread_create(struct thread_context * context)
{
	void * entrypoint = context_get_syscall_argument(context, 0);
	void * stack_ptr = context_get_syscall_argument(context, 1);

	struct thread * new_thread = process_add_new_thread(active_thread->parent, entrypoint, stack_ptr);
	scheduler_add_thread(new_thread);

	context_set_syscall_retval(context, (void *) new_thread->tid);
}

void syscall_thread_join(struct thread_context * context)
{
	tid_t retval = (tid_t) context_get_syscall_argument(context, 0);
	debug_printf("PID %d, TID %d wants to join with TID %d\n", active_thread->parent->pid,
		active_thread->tid, (tid_t) context_get_syscall_argument(context, 0));

	struct thread * join_thread = process_get_thread_by_tid(active_thread->parent,
		(tid_t) context_get_syscall_argument(context, 0));
	if(join_thread == 0)
	{
		context_set_syscall_retval(active_thread->context, (void *) -1);
		return;
	} else {
		thread_add_exit_listener(join_thread, active_thread);
		scheduler_move_thread_to_blocking(active_thread);
		scheduler_reschedule();
	}
}

void syscall_thread_info(struct thread_context * context)
{
	int function = (int) context_get_syscall_argument(context, 0);

	switch(function)
	{
		case MORDAX_THREAD_INFO_GET_TID:
			context_set_syscall_retval(context, (void *) active_thread->tid);
			break;
		case MORDAX_THREAD_INFO_GET_PID:
			context_set_syscall_retval(context, (void *) active_thread->parent->pid);
			break;
		case MORDAX_THREAD_INFO_GET_UID:
			context_set_syscall_retval(context, (void *) active_thread->parent->owner_user);
			break;
		case MORDAX_THREAD_INFO_GET_GID:
			context_set_syscall_retval(context, (void *) active_thread->parent->owner_group);
			break;
		default:
			context_set_syscall_retval(context, (void *) -1);
			break;
	}
}

