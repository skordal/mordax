// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "kernel.h"
#include "mm.h"
#include "process.h"
#include "rbtree.h"
#include "scheduler.h"

// Allocates a thread ID for a thread added to the process.
static tid_t allocate_tid(struct process * p, struct thread * t);
// Frees a thread ID for a thread.
static void free_tid(struct process * p, tid_t tid);

struct process * process_create(struct mordax_memory_map * memory_map, uid_t uid, gid_t gid, uint32_t permissions)
{
	struct process * retval = mm_allocate(sizeof(struct process), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	retval->num_threads = 0;
	retval->threads = queue_new();
	retval->pid = scheduler_allocate_pid();

	retval->next_tid = 0;
	retval->allocated_tids = rbtree_new(0, 0, 0);

	retval->owner_group = gid;
	retval->owner_user = uid;
	retval->permissions = permissions;

	debug_printf("Creating process translation table...\n");
	retval->translation_table = mmu_create_translation_table();

	debug_printf("Setting translation table as current...\n");
	mmu_set_user_translation_table(retval->translation_table, 0 /* PID does not matter here */);

	if(memory_map != 0)
	{
		for(int i = 0; i < memory_map->num_zones; ++i)
		{
			// FIXME: currently only allocating one page at a time -> incredibly slow
			// FIXME: write quicker allocator, but also allocate bigger chunks at a time
			struct mm_physical_memory zone;
			for(int p = 0; p < (memory_map->zones[i].size + CONFIG_PAGE_SIZE - 1) / CONFIG_PAGE_SIZE; ++p)
			{
				if(!mm_allocate_physical(CONFIG_PAGE_SIZE, &zone))
				{
					// TODO: free allocated/mapped memory.
					debug_printf("Cannot create process, out of memory!\n");
					return 0;
				}

				// Map the allocated memory:
				debug_printf("Mapping %x to %x\n", zone.base, (uint32_t) memory_map->zones[i].base + p * CONFIG_PAGE_SIZE);
				mmu_map(zone.base, (void *) ((uint32_t) memory_map->zones[i].base + p * CONFIG_PAGE_SIZE),
					zone.size, memory_map->zones[i].attributes.type, memory_map->zones[i].attributes.permissions);
			}
		}
	}

	return retval;
}

void process_free(struct process * p)
{
	debug_printf("Freeing process %d\n", p->pid);
	queue_free(p->threads, (queue_data_free_func) thread_free);
	scheduler_free_pid(p->pid);
	rbtree_free(p->allocated_tids, 0);
	// TODO: add support for freeing the translation table and physical memory
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

