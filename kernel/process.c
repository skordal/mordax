// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "kernel.h"
#include "mm.h"
#include "process.h"
#include "rbtree.h"
#include "scheduler.h"
#include "service.h"
#include "utils.h"

struct process_resource
{
	enum process_resource_type type;
	int identifier;
	void * resource_ptr;
};

// Allocates a thread ID for a thread added to the process.
static tid_t allocate_tid(struct process * p, struct thread * t);
// Frees a thread ID for a thread.
static void free_tid(struct process * p, tid_t tid);
// Frees a resource:
static void free_resource(void * data);

struct process * process_create(struct mordax_process_info * procinfo)
{
	struct process * retval = mm_allocate(sizeof(struct process), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	retval->num_threads = 0;
	retval->threads = queue_new();
	retval->pid = scheduler_allocate_pid();

	retval->next_tid = 0;
	retval->allocated_tids = rbtree_new(0, 0, 0, 0);

	retval->resource_table = rbtree_new(0, 0, 0, free_resource);
	retval->resnum_allocator = number_allocator_new();

	retval->owner_group = procinfo->gid;
	retval->owner_user = procinfo->uid;

	if((procinfo->permissions & MORDAX_PROCESS_INHERIT_PERMISSIONS) == MORDAX_PROCESS_INHERIT_PERMISSIONS)
		retval->permissions = active_thread->parent->permissions;
	else
		retval->permissions = procinfo->permissions;
	retval->translation_table = mmu_create_translation_table(retval->pid);

	// Create the initial stack:
	if(procinfo->stack_length == MORDAX_PROCESS_INHERIT_STACK_SIZE)
		retval->stack_size = active_thread->parent->stack_size;
	else
		retval->stack_size = (procinfo->stack_length + CONFIG_PAGE_SIZE - 1) & -CONFIG_PAGE_SIZE;
	int stack_pages = retval->stack_size / CONFIG_PAGE_SIZE;

	mmu_set_translation_table(retval->translation_table);
	for(int i = 0; i < stack_pages; ++i)
	{
		struct mm_physical_memory mem;
		if(!mm_allocate_physical(CONFIG_PAGE_SIZE, &mem))
		{
			debug_printf("Error: cannot allocate physical memory for stack!\n");
			goto _error_return;
		}

		debug_printf("\tMapping %x to %x for stack\n", mem.base,
			PROCESS_DEFAULT_STACK_TOP - ((i + 1) * CONFIG_PAGE_SIZE));
		mmu_map(retval->translation_table, mem.base,
			(void *) (PROCESS_DEFAULT_STACK_TOP - ((i + 1) * CONFIG_PAGE_SIZE)),
			mem.size, MORDAX_TYPE_STACK, MORDAX_PERM_RW_RW);
	}

	// Copy the initial stack contents:
	if(procinfo->stack_init_length > 0)
	{
		if(active_thread != 0 && !mmu_access_permitted(active_thread->parent->translation_table,
			procinfo->stack_init, procinfo->stack_init_length, MMU_ACCESS_READ|MMU_ACCESS_USER))
		{
			debug_printf("Error: cannot copy initial stack contents, access to memory is forbidden!\n");
			goto _error_return;
		} else {
			if(active_thread == 0)
			{
				mmu_set_translation_table(retval->translation_table);
				memcpy((void *) (PROCESS_DEFAULT_STACK_TOP - procinfo->stack_init_length),
					procinfo->stack_init, procinfo->stack_init_length);
			} else
				memcpy_p((void *) (PROCESS_DEFAULT_STACK_TOP - procinfo->stack_init_length), retval,
					procinfo->stack_init, active_thread->parent, procinfo->stack_init_length);
		}
	}

	// Create the process image:
	size_t image_size = (procinfo->source_length + CONFIG_PAGE_SIZE - 1) & -CONFIG_PAGE_SIZE;
	int text_pages = procinfo->text_length / CONFIG_PAGE_SIZE;
	int rodata_pages = procinfo->rodata_length / CONFIG_PAGE_SIZE;
	int data_pages = procinfo->data_length / CONFIG_PAGE_SIZE;

	if(text_pages + rodata_pages + data_pages < image_size / CONFIG_PAGE_SIZE)
	{
		debug_printf("Error: mismatch between section sizes and image size\n");
		goto _error_return;
	}

	// Allocate process memory:
	for(int i = 0; i < text_pages + rodata_pages + data_pages; ++i)
	{
		const int rodata_boundary = 1 + text_pages;
		const int data_boundary = 1 + text_pages + rodata_pages;

		struct mm_physical_memory mem;
		if(!mm_allocate_physical(CONFIG_PAGE_SIZE, &mem))
		{
			debug_printf("Error: cannot allocate physical memory for process image!\n");
			goto _error_return;
		}

		enum mordax_memory_type type;
		enum mordax_memory_permissions permissions;

		if(i + 1 >= data_boundary)
		{
			type = MORDAX_TYPE_DATA;
			permissions = MORDAX_PERM_RW_RW;
			debug_printf("Mapping data memory:\n\t");
		} else if(i + 1 >= rodata_boundary)
		{
			type = MORDAX_TYPE_RODATA;
			permissions = MORDAX_PERM_RW_RO;
			debug_printf("Mapping rodata memory:\n\t");
		} else {
			type = MORDAX_TYPE_CODE;
	//		permissions = MORDAX_PERM_RW_RO;
			permissions = MORDAX_PERM_RW_RW;
			debug_printf("Mapping code memory:\n\t");
		}

		debug_printf("Mapping %x to %x\n", mem.base, (i + 1) * CONFIG_PAGE_SIZE);
		mmu_map(retval->translation_table, mem.base, (void *) ((i + 1) * CONFIG_PAGE_SIZE),
			mem.size, type, permissions);
	}

	// Copy the process image:
	if(procinfo->source_length > 0)
	{
		if(active_thread != 0 && !mmu_access_permitted(active_thread->parent->translation_table,
			procinfo->source, procinfo->source_length, MMU_ACCESS_READ|MMU_ACCESS_USER))
		{
			debug_printf("Error: cannot copy process image, access to memory is forbidden!\n");
			goto _error_return;
		} else {
			if(active_thread == 0)
			{
				mmu_set_translation_table(retval->translation_table);
				memcpy((void *) 0x1000, procinfo->source, procinfo->source_length);
			} else
				memcpy_p((void *) 0x1000, retval, procinfo->source,
					active_thread->parent, procinfo->source_length);
		}
	}

	return retval;

_error_return:
	rbtree_free(retval->allocated_tids);
	scheduler_free_pid(retval->pid);
	queue_free(retval->threads, 0);
	mmu_free_translation_table(retval->translation_table);

	mm_free(retval);
	return 0;
}

void process_free(struct process * p)
{
	rbtree_free(p->resource_table); // also frees the resources in the table
	queue_free(p->threads, (queue_data_free_func) thread_free);
	number_allocator_free(p->resnum_allocator);
	scheduler_free_pid(p->pid);
	rbtree_free(p->allocated_tids);
	mmu_free_translation_table(p->translation_table);
	mm_free(p);
}

static void free_resource(void * data)
{
	struct process_resource * res = data;
	if(res == 0)
		return;

	switch(res->type)
	{
		case PROCESS_RESOURCE_SOCKET:
			socket_destroy(res->resource_ptr);
			break;
		case PROCESS_RESOURCE_SERVICE:
			service_destroy(res->resource_ptr);
			break;
		default:
			break;
	}

	mm_free(res);
}

void process_add_thread(struct process * p, struct thread * t)
{
	queue_add_front(p->threads, t);
	++p->num_threads;
	t->tid = allocate_tid(p, t);
	debug_printf("Added thread with TID %d to process %d, number of threads in process is now %d\n",
		t->tid, p->pid, p->num_threads);
}

struct thread * process_add_new_thread(struct process * p, void * entry, void * stack)
{
	struct thread * new_thread = thread_create(p, entry, stack);
	process_add_thread(p, new_thread);
	return new_thread;
}

struct thread * process_remove_thread(struct process * p, struct thread * t)
{
	free_tid(p, t->tid);

	struct queue_node * current = p->threads->first;
	while(current != 0)
	{
		if(current->data != t)
			current = current->next;
		else
			break;
	}

	if(current != 0)
	{
		debug_printf("Thread %d removed from process %d\n", t->tid, p->pid);
		queue_remove_node(p->threads, current);
		--p->num_threads;
	}

	if(p->num_threads == 0)
	{
		debug_printf("Last thread removed from process, freeing process %d\n", p->pid);
		process_free(p);
		t->parent = 0;
	}

	return t;
}

unsigned int process_add_resource(struct process * p, enum process_resource_type type,
	void * resource_ptr)
{
	unsigned int identifier = number_allocator_allocate_num(p->resnum_allocator);
	if(identifier == 0)
		return 0;

	struct process_resource * res = mm_allocate(sizeof(struct process_resource),
		MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);

	res->type = type;
	res->identifier = identifier;
	res->resource_ptr = resource_ptr;

	rbtree_insert(p->resource_table, (void *) identifier, (void *) res);
	return identifier;
}

void * process_get_resource(struct process * p, unsigned int identifier,
	enum process_resource_type * type)
{
	struct process_resource * res = rbtree_get_value(p->resource_table, (void *) identifier);
	if(res == 0)
		return 0;

	*type = res->type;
	return res->resource_ptr;
}

void * process_remove_resource(struct process * p, unsigned int identifier,
	enum process_resource_type * type)
{
	struct process_resource * resource = rbtree_delete(p->resource_table, (void *) identifier);
	void * retval = 0;

	if(resource != 0)
	{
		number_allocator_free_num(p->resnum_allocator, resource->identifier);
		*type = resource->type;
		retval = resource->resource_ptr;
		mm_free(resource);
	}

	return retval;
}

struct thread * process_get_thread_by_tid(struct process * p, tid_t tid)
{
	if(!rbtree_key_exists(p->allocated_tids, (void *) tid))
		return 0;
	else
		return rbtree_get_value(p->allocated_tids, (void *) tid);
}

static tid_t allocate_tid(struct process * p, struct thread * t)
{
	tid_t retval = p->next_tid++;
	tid_t first_attempt = retval;

	while(rbtree_key_exists(p->allocated_tids, (void *) retval))
	{
		retval = p->next_tid++;

		// Check if all possibilities have been tried:
		if(first_attempt == retval)
			return -1;
	}

	rbtree_insert(p->allocated_tids, (void *) retval, t);
	debug_printf("Process %d allocated TID %d\n", p->pid, retval);
	return retval;
}

static void free_tid(struct process * p, tid_t tid)
{
	debug_printf("Process %d freed TID %d\n", p->pid, tid);
	rbtree_delete(p->allocated_tids, (void *) tid);
}

