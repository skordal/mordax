// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "context.h"
#include "debug.h"
#include "process.h"
#include "scheduler.h"
#include "syscall.h"
#include "thread.h"

// System call handler, called by target assembly code:
void syscall_interrupt_handler(struct thread_context * context, uint8_t syscall)
{
	void * arguments[3] = {
		context_get_syscall_argument(context, 0),
		context_get_syscall_argument(context, 1),
		context_get_syscall_argument(context, 2)};

	debug_printf("System call from process %d thread %d: %d\n",
		active_thread->parent->pid, active_thread->tid, syscall);

	switch(syscall)
	{
		case SYSCALL_SYSTEM:
			syscall_system(context, (int) arguments[0]);
			break;
		case SYSCALL_THREAD_EXIT:
			syscall_thread_exit(context);
			break;
		default: // TODO: handle unrecognized system calls
			debug_printf("Unknown system call %d\n", syscall);
			break;
	}
}

void syscall_system(struct thread_context * context, int function)
{
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

	thread_free(removed_thread);
	scheduler_reschedule();
}

