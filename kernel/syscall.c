// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "context.h"
#include "debug.h"
#include "dt.h"
#include "kernel.h"
#include "lock.h"
#include "mm.h"
#include "process.h"
#include "scheduler.h"
#include "service.h"
#include "syscall.h"
#include "thread.h"
#include "utils.h"

#include "api/dt.h"
#include "api/errno.h"
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
		case MORDAX_SYSCALL_THREAD_YIELD:
			scheduler_reschedule();
			break;
		case MORDAX_SYSCALL_THREAD_INFO:
			syscall_thread_info(context);
			break;

		case MORDAX_SYSCALL_PROCESS_CREATE:
			syscall_process_create(context);
			break;

		case MORDAX_SYSCALL_MAP:
			syscall_memory_map(context);
			break;
		case MORDAX_SYSCALL_MAP_ALLOC:
			syscall_memory_map_alloc(context);
			break;
		case MORDAX_SYSCALL_UNMAP:
			syscall_memory_unmap(context);
			break;

		case MORDAX_SYSCALL_SERVICE_CREATE:
			syscall_service_create(context);
			break;
		case MORDAX_SYSCALL_SERVICE_LISTEN:
			syscall_service_listen(context);
			break;
		case MORDAX_SYSCALL_SERVICE_CONNECT:
			syscall_service_connect(context);
			break;

		case MORDAX_SYSCALL_SOCKET_SEND:
			syscall_socket_send(context);
			break;
		case MORDAX_SYSCALL_SOCKET_RECEIVE:
			syscall_socket_receive(context);
			break;
		case MORDAX_SYSCALL_SOCKET_WAIT:
			syscall_socket_wait(context);
			break;

		case MORDAX_SYSCALL_LOCK_CREATE:
			syscall_lock_create(context);
			break;
		case MORDAX_SYSCALL_LOCK_AQUIRE:
			syscall_lock_aquire(context);
			break;
		case MORDAX_SYSCALL_LOCK_RELEASE:
			syscall_lock_release(context);
			break;

		case MORDAX_SYSCALL_DT_GET_NODE_BY_PATH:
			syscall_dt_get_node_by_path(context);
			break;
		case MORDAX_SYSCALL_DT_GET_NODE_BY_PHANDLE:
			syscall_dt_get_node_by_phandle(context);
			break;
		case MORDAX_SYSCALL_DT_GET_NODE_BY_COMPATIBLE:
			syscall_dt_get_node_by_compatible(context);
			break;
		case MORDAX_SYSCALL_DT_GET_PROPERTY_ARRAY32:
			syscall_dt_get_property_array32(context);
			break;
		case MORDAX_SYSCALL_DT_GET_PROPERTY_STRING:
			syscall_dt_get_property_string(context);
			break;
		case MORDAX_SYSCALL_DT_GET_PROPERTY_PHANDLE:
			syscall_dt_get_property_phandle(context);
			break;

		case MORDAX_SYSCALL_RESOURCE_DESTROY:
			syscall_resource_destroy(context);
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
	scheduler_reschedule();
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
	void * retval = 0;

	// Check for permission to create a new process:
	if((active_thread->parent->permissions & MORDAX_PROCESS_PERMISSION_CREATE_PROC) == 0)
	{
		debug_printf("Error: cannot create process, calling process lacks permission to do so\n");
		context_set_syscall_retval(context, (void *) -EPERM);
		return;
	}

	if(!mmu_access_permitted(0, procinfo_ptr, sizeof(struct mordax_process_info), MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		// TODO: terminate calling process.
		debug_printf("Error: cannot create process: cannot access process info structure\n");
		retval = (void *) -EFAULT;
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
		retval = (void *) -ENOEXEC;
		goto _error_return;
	}

	// Create the initial thread:
	init_thread = process_add_new_thread(proc, procinfo_cpy.entry_point, (void *) PROCESS_DEFAULT_STACK_TOP);
	if(init_thread == 0)
	{
		debug_printf("Error: could not create process: could not create thread\n");
		retval = (void *) -ENOEXEC;
		goto _error_return;
	}

	scheduler_add_thread(init_thread);
	context_set_syscall_retval(context, (void *) proc->pid);
	scheduler_reschedule();
	return;

_error_return:
	if(init_thread != 0)
		thread_free(init_thread, -1); // also frees the process
	else if(proc != 0)
		process_free(proc);
	context_set_syscall_retval(context, retval);
}

void syscall_memory_map(struct thread_context * context)
{
	// Check for required permissions:
	if((active_process->permissions & MORDAX_PROCESS_PERMISSION_MAP_MEMORY) == 0)
	{
		debug_printf("Error: cannot map memory, calling process lacks permissions to do so\n");
		context_set_syscall_retval(context, (void *) -EPERM);
		return;
	}

	void * target = context_get_syscall_argument(context, 0);
	physical_ptr * source = context_get_syscall_argument(context, 1);
	size_t size = ((uint32_t) context_get_syscall_argument(context, 2) + CONFIG_PAGE_SIZE - 1) & -CONFIG_PAGE_SIZE;
	struct mordax_memory_attributes * attributes = context_get_syscall_argument(context, 3);

	if(!mmu_access_permitted(0, attributes, sizeof(struct mordax_memory_attributes), MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		debug_printf("Error: cannot map memory, cannot access attributes structure\n");
		context_set_syscall_retval(context, (void *) -EFAULT);
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
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}
	if(mm_is_physical_managed(start_physical))
	{
		debug_printf("Error: cannot map memory managed by the physical memory manager\n");
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	// TODO: do better checking of these addresses, as the address space to map can overlap important stuff.

	void * retval = mmu_map(active_thread->parent->translation_table, start_physical,
		start_virtual, real_size, attributes->type, attributes->permissions);
	context_set_syscall_retval(context, retval);
	mmu_invalidate();
}

void syscall_memory_map_alloc(struct thread_context * context)
{
	void * target = (void *) ((uint32_t) context_get_syscall_argument(context, 0) & -CONFIG_PAGE_SIZE);
	size_t * size = context_get_syscall_argument(context, 1);
	struct mordax_memory_attributes * attributes = context_get_syscall_argument(context, 2);

	if(!mmu_access_permitted(0, size, sizeof(size_t), MMU_ACCESS_READ|MMU_ACCESS_WRITE|MMU_ACCESS_USER))
	{
		debug_printf("Error: cannot map memory, cannot access memory area size\n");
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	if(!mmu_access_permitted(0, attributes, sizeof(struct mordax_memory_attributes), MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		debug_printf("Error: cannot map memory, cannot access memory attributes\n");
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	if(*size > MM_MAXIMUM_PHYSICAL_BLOCK_SIZE)
	{
		debug_printf("Error: cannot map memory, memory request too large\n");
		context_set_syscall_retval(context, (void *) -ENOMEM);
		return;
	}

	if((uint32_t) target > CONFIG_KERNEL_SPLIT)
	{
		debug_printf("Error: cannot map memory, target address is in kernel space\n");
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	struct mm_physical_memory allocation;
	mm_allocate_physical(*size, &allocation);
	*size = allocation.size;

	void * retval = mmu_map(active_thread->parent->translation_table, allocation.base, target,
		allocation.size, attributes->type, attributes->permissions);
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

void syscall_service_create(struct thread_context * context)
{
	const char * name = context_get_syscall_argument(context, 0);
	size_t name_length = (size_t) context_get_syscall_argument(context, 1);

	if((active_process->permissions & MORDAX_PROCESS_PERMISSION_SERVICE) == 0)
	{
		debug_printf("Error: cannot create service, not permitted\n");
		context_set_syscall_retval(context, (void *) -EPERM);
		return;
	}

	if(!mmu_access_permitted(0, (void *) name, name_length, MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		debug_printf("Error: cannot access service name, memory access denied\n");
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	char * service_name = mm_allocate(name_length + 1,
		MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	memclr(service_name, name_length + 1);
	memcpy(service_name, name, name_length);

	struct service * retval = service_create(service_name, active_process);
	if(retval != 0)
		context_set_syscall_retval(context,
			(void *) process_add_resource(active_process, PROCESS_RESOURCE_SERVICE, retval));
	else
		context_set_syscall_retval(context, (void *) -ENOMEM); // FIXME: may also be due to invalid/existing service name.

	mm_free(service_name);
}

void syscall_service_listen(struct thread_context * context)
{
	mordax_resource_t identifier = (mordax_resource_t) context_get_syscall_argument(context, 0);
	debug_printf("PID %d, TID %d wants to listen to service %d\n",
		active_process->pid, active_thread->tid, identifier);

	if((active_process->permissions & MORDAX_PROCESS_PERMISSION_SERVICE) == 0)
	{
		debug_printf("Error: cannot listen on service, operation not permitted\n");
		context_set_syscall_retval(context, (void *) -EPERM);
		return;
	}

	enum process_resource_type restype;
	struct service * svc = process_get_resource(active_process, identifier, &restype);
	if(svc == 0)
	{
		debug_printf("Error: no resource associated with identifier %d\n",
			identifier);
		context_set_syscall_retval(context, (void *) -EINVAL);
		return;
	} else if(restype != PROCESS_RESOURCE_SERVICE)
	{
		debug_printf("Error: resource associated with identifier %d is not a service\n",
			identifier);
		context_set_syscall_retval(context, (void *) -EINVAL);
		return;
	}

	bool blocking = false;
	struct socket * server_socket = 0;
	int retval = service_listen(svc, active_thread, &server_socket, &blocking);

	if(blocking)
	{
		scheduler_move_thread_to_blocking(active_thread);
		scheduler_reschedule();
	} else if(retval < 0)
	{
		context_set_syscall_retval(context, (void *) retval);
	} else {
		retval = process_add_resource(active_process, PROCESS_RESOURCE_SOCKET,
			server_socket);
		context_set_syscall_retval(context, (void *) retval);
	}
}

void syscall_service_connect(struct thread_context * context)
{
	char * name = context_get_syscall_argument(context, 0);
	size_t name_length = (size_t) context_get_syscall_argument(context, 1);

	if(!mmu_access_permitted(0, (void *) name, name_length, MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		debug_printf("Error: cannot access service name, memory access not permitted\n");
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	char * service_name = mm_allocate(name_length + 1, MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	memclr(service_name, name_length + 1);
	memcpy(service_name, name, name_length + 1);

	struct service * svc = service_lookup(service_name);
	if(svc == 0)
	{
		debug_printf("Error: the service \"%s\" does not exist\n");
		context_set_syscall_retval(context, (void *) -ENOENT);
		goto _cleanup;
	} else
		debug_printf("Connecting to service \"%s\"\n", service_name);

	struct socket * client_socket = 0;
	bool block = false;
	int retval = service_connect(svc, active_thread, &client_socket, &block);

	if(block)
	{
		scheduler_move_thread_to_blocking(active_thread);
		scheduler_reschedule();
	} else if(retval < 0)
	{
		context_set_syscall_retval(context, (void *) retval);
	} else {
		retval = process_add_resource(active_process, PROCESS_RESOURCE_SOCKET,
			client_socket);
		context_set_syscall_retval(context, (void *) retval);
	}

_cleanup:
	mm_free(service_name);
}

void syscall_socket_send(struct thread_context * context)
{
	mordax_resource_t identifier = (mordax_resource_t) context_get_syscall_argument(context, 0);
	const void * buffer = context_get_syscall_argument(context, 1);
	size_t buffer_length = (size_t) context_get_syscall_argument(context, 2);

	debug_printf("PID %d, TID %d wants to send %d bytes on socket %d\n", active_process->pid,
		active_thread->tid, buffer_length, identifier);

	if(!mmu_access_permitted(0, buffer, buffer_length, MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		debug_printf("Error: cannot send message, buffer pointer points to invalid memory\n");
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	enum process_resource_type restype;
	struct socket * send_socket = process_get_resource(active_process, identifier, &restype);
	if(restype != PROCESS_RESOURCE_SOCKET)
	{
		debug_printf("Error: cannot send message, resource is not a socket\n");
		context_set_syscall_retval(context, (void *) -ENOTSOCK);
		return;
	}

	if(send_socket->endpoint == 0)
	{
		debug_printf("Error: cannot send message, socket is not connected\n");
		context_set_syscall_retval(context, (void *) -ENOTCONN);
		return;
	}

	bool block = false;
	int retval = socket_send(send_socket, active_thread, buffer, buffer_length, &block);

	if(block)
	{
		scheduler_move_thread_to_blocking(active_thread);
		scheduler_reschedule();
	} else
		context_set_syscall_retval(context, (void *) retval);
}

void syscall_socket_receive(struct thread_context * context)
{
	mordax_resource_t identifier = (mordax_resource_t) context_get_syscall_argument(context, 0);
	void * buffer = context_get_syscall_argument(context, 1);
	size_t buffer_length = (size_t) context_get_syscall_argument(context, 2);

	debug_printf("PID %d, TID %d wants to receive %d bytes on socket %d\n", active_process->pid,
		active_thread->tid, buffer_length, identifier);

	if(!mmu_access_permitted(0, buffer, buffer_length, MMU_ACCESS_WRITE|MMU_ACCESS_USER))
	{
		debug_printf("Error: cannot receive message, buffer pointer points to invalid memory\n");
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	enum process_resource_type restype;
	struct socket * receive_socket = process_get_resource(active_process, identifier, &restype);
	if(restype != PROCESS_RESOURCE_SOCKET)
	{
		debug_printf("Error: cannot receive message, resource is not a socket\n");
		context_set_syscall_retval(context, (void *) -ENOTSOCK);
		return;
	}

	if(receive_socket->endpoint == 0)
	{
		debug_printf("Error: cannot receive message, socket is not connected\n");
		context_set_syscall_retval(context, (void *) -ENOTCONN);
		return;
	}

	bool block = false;
	int retval = socket_receive(receive_socket, active_thread, buffer, buffer_length, &block);

	if(block)
	{
		scheduler_move_thread_to_blocking(active_thread);
		scheduler_reschedule();
	} else
		context_set_syscall_retval(context, (void *) retval);
}

void syscall_socket_wait(struct thread_context * context)
{
	mordax_resource_t identifier = (mordax_resource_t) context_get_syscall_argument(context, 0);

	debug_printf("PID %d, TID %d waiting for a message on socket %d\n", active_process->pid,
		active_thread->tid, identifier);

	enum process_resource_type restype;
	struct socket * wait_socket = process_get_resource(active_process, identifier, &restype);
	if(restype != PROCESS_RESOURCE_SOCKET)
	{
		debug_printf("Error: cannot wait on resource %d, resource is not a socket\n",
			identifier);
		context_set_syscall_retval(context, (void *) -ENOTSOCK);
		return;
	}

	if(wait_socket->endpoint == 0)
	{
		debug_printf("Error: cannot wait on socket, socket is not connected\n");
		context_set_syscall_retval(context, (void *) -ENOTCONN);
		return;
	}

	bool block = false;
	int retval = socket_wait(wait_socket, active_thread, &block);
	if(block)
	{
		scheduler_move_thread_to_blocking(active_thread);
		scheduler_reschedule();
	} else
		context_set_syscall_retval(context, (void *) retval);
}

void syscall_lock_create(struct thread_context * context)
{
	if((active_process->permissions & MORDAX_PROCESS_PERMISSION_LOCKS) == 0)
	{
		context_set_syscall_retval(context, (void *) -EPERM);
		return;
	}

	struct lock * new_lock = lock_create();
	if(new_lock == 0)
	{
		context_set_syscall_retval(context, (void *) -ENOMEM);
		return;
	}

	mordax_resource_t retval = process_add_resource(active_process, PROCESS_RESOURCE_LOCK, new_lock);
	context_set_syscall_retval(context, (void *) retval);
}

void syscall_lock_aquire(struct thread_context * context)
{
	if((active_process->permissions & MORDAX_PROCESS_PERMISSION_LOCKS) == 0)
	{
		context_set_syscall_retval(context, (void *) -EPERM);
		return;
	}

	mordax_resource_t identifier = (mordax_resource_t) context_get_syscall_argument(context, 0);

	enum process_resource_type restype;
	struct lock * l = process_get_resource(active_process, identifier, &restype);
	if(restype != PROCESS_RESOURCE_LOCK)
	{
		context_set_syscall_retval(context, (void *) -EINVAL);
		return;
	}

	bool blocking = false;
	int retval = lock_aquire(l, active_thread, &blocking);
	if(retval != 0)
		context_set_syscall_retval(context, (void *) retval);
	 else if(blocking)
		scheduler_reschedule();
	else
		context_set_syscall_retval(context, (void *) retval);
}

void syscall_lock_release(struct thread_context * context)
{
	if((active_process->permissions & MORDAX_PROCESS_PERMISSION_LOCKS) == 0)
	{
		context_set_syscall_retval(context, (void *) -EPERM);
		return;
	}

	mordax_resource_t identifier = (mordax_resource_t) context_get_syscall_argument(context, 0);

	enum process_resource_type restype;
	struct lock * l = process_get_resource(active_process, identifier, &restype);
	if(restype != PROCESS_RESOURCE_LOCK)
	{
		context_set_syscall_retval(context, (void *) -EINVAL);
		return;
	}

	context_set_syscall_retval(context, (void *) lock_release(l, active_thread));
}

void syscall_dt_get_node_by_path(struct thread_context * context)
{
	char * path = context_get_syscall_argument(context, 0);
	size_t path_length = (size_t) context_get_syscall_argument(context, 1);

	if(!mmu_access_permitted(0, path, path_length, MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	char * path_string = mm_allocate(path_length + 1, MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	memcpy(path_string, path, path_length);
	path_string[path_length] = 0;

	struct dt_node * node = dt_get_node_by_path(kernel_dt, path_string);
	if(node == 0)
		context_set_syscall_retval(context, (void *) -ENOENT);
	else {
		mordax_resource_t retval = process_add_resource(active_process, PROCESS_RESOURCE_DT_NODE,
			node);
		context_set_syscall_retval(context, (void *) retval);
	}

	mm_free(path_string);
}

void syscall_dt_get_node_by_phandle(struct thread_context * context)
{
	dt_phandle phandle = (dt_phandle) context_get_syscall_argument(context, 0);

	struct dt_node * node = dt_get_node_by_phandle(kernel_dt, phandle);
	if(node == 0)
		context_set_syscall_retval(context, (void *) -ENOENT);
	else {
		mordax_resource_t retval = process_add_resource(active_process, PROCESS_RESOURCE_DT_NODE,
			node);
		context_set_syscall_retval(context, (void *) retval);
	}
}

void syscall_dt_get_node_by_compatible(struct thread_context * context)
{
	struct mordax_dt_string * compatible = context_get_syscall_argument(context, 0);
	int index = (int) context_get_syscall_argument(context, 1);

	if(!mmu_access_permitted(0, compatible, sizeof(struct mordax_dt_string), MMU_ACCESS_READ|MMU_ACCESS_USER)
		|| !mmu_access_permitted(0, compatible->string, compatible->length, MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	char * compatible_string = mm_allocate(compatible->length + 1, MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	memcpy(compatible_string, compatible->string, compatible->length);
	compatible_string[compatible->length] = 0;

	struct dt_node * node = dt_get_node_by_compatible(kernel_dt, compatible_string, index);
	if(node == 0)
		context_set_syscall_retval(context, (void *) -ENOENT);
	else {
		mordax_resource_t retval = process_add_resource(active_process, PROCESS_RESOURCE_DT_NODE,
			node);
		context_set_syscall_retval(context, (void *) retval);
	}

	mm_free(compatible_string);
}

void syscall_dt_get_property_array32(struct thread_context * context)
{
	mordax_resource_t identifier = (mordax_resource_t) context_get_syscall_argument(context, 0);
	struct mordax_dt_string * name = context_get_syscall_argument(context, 1);
	uint32_t * out_array = context_get_syscall_argument(context, 2);
	size_t out_length = (size_t) context_get_syscall_argument(context, 3);

	// Check memory accesses:
	if(!mmu_access_permitted(0, out_array, out_length * sizeof(uint32_t),
		MMU_ACCESS_WRITE|MMU_ACCESS_USER))
	{
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	if(!mmu_access_permitted(0, name, sizeof(struct mordax_dt_string), MMU_ACCESS_READ|MMU_ACCESS_USER)
		|| !mmu_access_permitted(0, name->string, name->length, MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	// Get the device tree node:
	enum process_resource_type restype;
	struct dt_node * node = process_get_resource(active_process, identifier, &restype);
	if(restype != PROCESS_RESOURCE_DT_NODE || node == 0)
	{
		context_set_syscall_retval(context, (void *) -EINVAL);
		return;
	}

	// Copy the property name:
	char * real_name = mm_allocate(name->length + 1, MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	memcpy(real_name, name->string, name->length);
	real_name[name->length] = 0;

	if(!dt_get_array32_property(node, real_name, out_array, out_length))
		context_set_syscall_retval(context, (void *) -ENOENT);
	else
		context_set_syscall_retval(context, 0);

	mm_free(real_name);
}

void syscall_dt_get_property_string(struct thread_context * context)
{
	mordax_resource_t identifier = (mordax_resource_t) context_get_syscall_argument(context, 0);
	struct mordax_dt_string * name = context_get_syscall_argument(context, 1);
	struct mordax_dt_string * ret = context_get_syscall_argument(context, 2);

	// Check access permissions for the name structure and the name string:
	if(!mmu_access_permitted(0, name, sizeof(struct mordax_dt_string), MMU_ACCESS_READ|MMU_ACCESS_USER)
		|| !mmu_access_permitted(0, name->string, name->length, MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	// Check access permissions for the return string:
	if(!mmu_access_permitted(0, ret, sizeof(struct mordax_dt_string), MMU_ACCESS_READ|MMU_ACCESS_WRITE|MMU_ACCESS_USER)
		|| !mmu_access_permitted(0, ret->string, ret->length, MMU_ACCESS_WRITE|MMU_ACCESS_USER))
	{
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	size_t maxlen = ret->length;

	// Get the device tree node:
	enum process_resource_type restype;
	struct dt_node * node = process_get_resource(active_process, identifier, &restype);
	if(restype != PROCESS_RESOURCE_DT_NODE || node == 0)
	{
		context_set_syscall_retval(context, (void *) -EINVAL);
		return;
	}

	// Copy the property name:
	char * real_name = mm_allocate(name->length + 1, MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	memcpy(real_name, name->string, name->length);
	real_name[name->length] = 0;

	const char * stringval = dt_get_string_property(node, real_name);
	if(stringval == 0)
	{
		context_set_syscall_retval(context, (void *) -ENOENT);
	} else {
		size_t proplength = strlen(stringval);
		if(proplength >= maxlen)
		{
			memcpy(ret->string, stringval, maxlen - 1);
			ret->string[maxlen] = 0;
			ret->length = maxlen - 1;
		} else {
			memcpy(ret->string, stringval, strlen(stringval));
			ret->string[strlen(stringval)] = 0;
			ret->length = strlen(stringval);
		}
		context_set_syscall_retval(context, 0);
	}

	mm_free(real_name);
}

void syscall_dt_get_property_phandle(struct thread_context * context)
{
	mordax_resource_t identifier = (mordax_resource_t) context_get_syscall_argument(context, 0);
	struct mordax_dt_string * name = context_get_syscall_argument(context, 1);
	unsigned int * ret = context_get_syscall_argument(context, 2);

	if(!mmu_access_permitted(0, name, sizeof(struct mordax_dt_string), MMU_ACCESS_READ|MMU_ACCESS_USER)
		|| !mmu_access_permitted(0, name->string, name->length, MMU_ACCESS_READ|MMU_ACCESS_USER))
	{
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	if(!mmu_access_permitted(0, ret, sizeof(unsigned int), MMU_ACCESS_WRITE|MMU_ACCESS_USER))
	{
		context_set_syscall_retval(context, (void *) -EFAULT);
		return;
	}

	enum process_resource_type restype;
	struct dt_node * node = process_get_resource(active_process, identifier, &restype);
	if(restype != PROCESS_RESOURCE_DT_NODE || node == 0)
	{
		context_set_syscall_retval(context, (void *) -EINVAL);
		return;
	}

	char * real_name = mm_allocate(name->length + 1, MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	memcpy(real_name, name->string, name->length);
	real_name[name->length] = 0;

	*ret = dt_get_phandle_property(node, real_name);
	context_set_syscall_retval(context, 0);
	mm_free(real_name);
}

void syscall_resource_destroy(struct thread_context * context)
{
	mordax_resource_t identifier = (mordax_resource_t) context_get_syscall_argument(context, 0);
	debug_printf("PID %d, TID %d wants to destroy resource %d\n",
		active_process->pid, active_thread->tid, identifier);

	enum process_resource_type restype;
	void * res = process_remove_resource(active_process, identifier, &restype);

	if(res == 0)
		return;

	switch(restype)
	{
		case PROCESS_RESOURCE_SERVICE:
			service_destroy(res);
			break;
		case PROCESS_RESOURCE_SOCKET:
			socket_destroy(res);
			break;
		case PROCESS_RESOURCE_LOCK:
			lock_destroy(res);
			break;
		default:
			context_set_syscall_retval(context, (void *) -EINVAL);
	}

	debug_printf("Service removed\n");
	context_set_syscall_retval(context, (void *) 0);
}

