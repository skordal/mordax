// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "context.h"
#include "debug.h"
#include "mm.h"
#include "process.h"
#include "scheduler.h"
#include "syscall.h"
#include "thread.h"
#include "utils.h"

#include "api/syscalls.h"
#include "api/system.h"
#include "api/thread.h"

// System call handler, called by target assembly code:
void syscall_interrupt_handler(struct thread_context * context, uint8_t syscall)
{
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
		case MORDAX_SYSCALL_PROCESS_CREATE:
			syscall_process_create(context);
			break;
		case MORDAX_SYSCALL_MAP:
			syscall_memory_map(context);
			break;
		case MORDAX_SYSCALL_UNMAP:
			syscall_memory_unmap(context);
			break;
		default: // TODO: handle unrecognized system calls
			debug_printf("Unknown system call %d\n", syscall);
			context_print(context);
			break;
	}
}

void syscall_system(struct thread_context * context)
{
	int function = (int) context_get_syscall_argument(context, 0);

	switch(function)
	{
		case MORDAX_SYSTEM_DEBUG:
		{
			const char * string = context_get_syscall_argument(context, 1);
			debug_printf("[SYSCALL0: DEBUG] PID %d, TID %d: %s\n", active_thread->parent->pid,
				active_thread->tid, string);
			break;
		}
		case MORDAX_SYSTEM_GETSPLIT:
			context_set_syscall_retval(context, (void *) CONFIG_KERNEL_SPLIT);
			break;
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

void syscall_process_create(struct thread_context * context)
{
	struct process * proc = 0;
	struct thread * init_thread = 0;
	struct mordax_process_info * procinfo_ptr = context_get_syscall_argument(context, 0);
	struct mordax_process_info procinfo_cpy;

	debug_printf("PID %d, TID %d wants to create a new process\n", active_thread->parent->pid,
		active_thread->tid);
	context_print(context);
	debug_printf("\tProcess info @ %p (copied to %p)\n", procinfo_ptr, &procinfo_cpy);

	if(!mmu_access_permitted(0, procinfo_ptr, sizeof(struct mordax_process_info), MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		// TODO: terminate calling process.
		debug_printf("Error: cannot create process: cannot access process info structure\n");
		goto _error_return;
	} else {
		// Copy the process info to allow access while switching between address spaces
		// during process creation.
		memcpy(&procinfo_cpy, procinfo_ptr, sizeof(struct mordax_process_info));
	}

	// Create the process:
	proc = process_create(&procinfo_cpy);
	if(proc == 0)
	{
		debug_printf("Error: could not create process: process_create failed\n");
		goto _error_return;
	}

	// Create the initial thread:
	init_thread = process_add_new_thread(proc, procinfo_cpy.entry_point, (void *) PROCESS_DEFAULT_STACK_TOP);
	if(init_thread == 0)
	{
		debug_printf("Error: could not create process: could not create initial thread\n");
		goto _error_return;
	}

	scheduler_add_thread(init_thread);
	context_set_syscall_retval(context, (void *) proc->pid);
//	scheduler_reschedule();
	return;

_error_return:
	if(init_thread != 0)
		thread_free(init_thread, -1); // also frees the process
	else if(proc != 0)
		process_free(proc);
	context_set_syscall_retval(context, (void *) -1);
}

void syscall_memory_map(struct thread_context * context)
{
	// Check for required permissions:
	if((active_thread->parent->permissions & MORDAX_PROCESS_PERMISSION_MAP_MEMORY) == 0)
	{
		debug_printf("Error: cannot map memory, calling process lacks permissions to do so\n");
		context_set_syscall_retval(context, (void *) -1);
		return;
	}

	void * target = context_get_syscall_argument(context, 0);
	physical_ptr * source = context_get_syscall_argument(context, 1);
	size_t size = ((uint32_t) context_get_syscall_argument(context, 2) + CONFIG_PAGE_SIZE - 1) & -CONFIG_PAGE_SIZE;
	struct mordax_memory_attributes * attributes = context_get_syscall_argument(context, 3);

	if(!mmu_access_permitted(0, attributes, sizeof(struct mordax_memory_attributes), MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		debug_printf("Error: cannot map memory, cannot access attributes structure\n");
		context_set_syscall_retval(context, 0);
		return;
	}

	physical_ptr start_physical = (void *) ((uint32_t) source & -CONFIG_PAGE_SIZE);
	void * start_virtual = (void *) ((uint32_t) target & -CONFIG_PAGE_SIZE);
	size_t real_size = ((((uint32_t) source + size) + CONFIG_PAGE_SIZE - 1) & -CONFIG_PAGE_SIZE)
		- ((uint32_t) source & -CONFIG_PAGE_SIZE);

	// Check the addresses:
	if((uint32_t) start_virtual >= CONFIG_KERNEL_SPLIT)
	{
		debug_printf("Error: target address cannot be in kernel space\n");
		context_set_syscall_retval(context, 0);
		return;
	}
	if(mm_is_physical_managed(start_physical))
	{
		debug_printf("Error: cannot map memory managed by the physical memory manager\n");
		context_set_syscall_retval(context, 0);
		return;
	}

	// TODO: do better checking of these addresses, as the address space to map can overlap important stuff.

	void * retval = mmu_map(active_thread->parent->translation_table, start_physical,
		start_virtual, real_size, attributes->type, attributes->permissions);
	context_set_syscall_retval(context, retval);
	mmu_invalidate();
}

void syscall_memory_unmap(struct thread_context * context)
{
	void * start_unmap = (void *) ((uint32_t) context_get_syscall_argument(context, 0) & -CONFIG_PAGE_SIZE);
	size_t size = ((uint32_t) context_get_syscall_argument(context, 1) + CONFIG_PAGE_SIZE - 1) & -CONFIG_PAGE_SIZE;

	// TODO: do better checking of what the memory area to unmap contains.
	if((uint32_t) start_unmap >= CONFIG_KERNEL_SPLIT)
	{
		debug_printf("Error: cannot unmap memory in the kernel's address space\n");
		return;
	}

	mmu_unmap(active_thread->parent->translation_table, start_unmap, size);
	mmu_invalidate();
}

