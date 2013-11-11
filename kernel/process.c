// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "kernel.h"
#include "mm.h"
#include "process.h"
#include "scheduler.h"

struct process * process_create(struct process_memory_map * memory_map)
{
	struct process * retval = mm_allocate(sizeof(struct process), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	retval->num_threads = 0;
	retval->threads = queue_new();
	retval->pid = scheduler_allocate_pid();

	retval->next_tid = 0;
	retval->allocated_tids = rbtree_new(0, 0, 0);

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
					zone.size, memory_map->zones[i].type, memory_map->zones[i].permissions);
			}
		}
	}

	return retval;
}

void process_free(struct process * p)
{
	queue_free(p->threads, (queue_data_free_func) thread_free);
	scheduler_free_pid(p->pid);
	rbtree_free(p->allocated_tids, 0);
	// TODO: add support for freeing the translation table and physical memory
}

void process_add_thread(struct process * p, struct thread * t)
{
	queue_add_front(p->threads, t);
	++p->num_threads;
}

struct thread * process_add_new_thread(struct process * p, void * entry, void * stack)
{
	struct thread * new_thread = thread_create(p, entry, stack);
	process_add_thread(p, new_thread);
	return new_thread;
}

tid_t process_allocate_tid(struct process * p)
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

	rbtree_insert(p->allocated_tids, (void *) retval, (void *) true);
	return retval;
}

void process_free_tid(struct process * p, tid_t tid)
{
	rbtree_delete(p->allocated_tids, (void *) tid);
}

