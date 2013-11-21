// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "api/memory.h"

#include "context.h"
#include "debug.h"
#include "kernel.h"
#include "process.h"
#include "queue.h"
#include "rbtree.h"
#include "scheduler.h"
#include "stack.h"
#include "utils.h"

static struct timer_driver * scheduler_timer;

// Thread queues:
static struct queue * running_queue;
static struct queue * blocking_queue;

// Next PID to allocate:
static pid_t next_pid = 0;
// Allocated PIDs:
static struct rbtree * allocated_pids;

// Currently running thread:
struct thread * active_thread = 0;

// Idle thread:
static struct thread * idle_thread;

// Idle thread loop:
static void idle_thread_loop(void) __attribute((naked, noreturn));

bool scheduler_initialize(struct timer_driver * timer, physical_ptr initproc_start,
	size_t initproc_size)
{
	scheduler_timer = timer;
	if(timer == 0)
	{
		debug_printf("Error: no scheduler timer available!\n");
		return false;
	}

	// Set up the timer:
	scheduler_timer->set_interval(1000000);
	scheduler_timer->set_callback(scheduler_reschedule);

	running_queue = queue_new();
	blocking_queue = queue_new();
	allocated_pids = rbtree_new(0, 0, 0);

	// Create the idle process + thread:
	struct process * idle_process = process_create(0);
	idle_thread = process_add_new_thread(idle_process, (void *) idle_thread_loop, 0);
	context_set_mode(idle_thread->context, CONTEXT_KERNELMODE);

	// Map the initial process:
	uint8_t * initproc_image = mmu_map(initproc_start, initproc_start, initproc_size,
		MMU_TYPE_DATA, MMU_PERM_RO_NA);

	// Create process memory map for the initial process; only two memory zones are needed,
	// one for the code and one for the stack.
	struct mordax_memory_zone initproc_datazones[] = {
		{
			.base = PROCESS_START_ADDRESS,
			.size = initproc_size,
			.type = MMU_TYPE_CODE,
			.permissions = MMU_PERM_RW_RW,
		},
		{
			.base = PROCESS_DEFAULT_STACK_BASE,
			.size = PROCESS_DEFAULT_STACK_SIZE,
			.type = MMU_TYPE_STACK,
			.permissions = MMU_PERM_RW_RW,
		}
	};
	struct mordax_memory_map initproc_memory_map = {
		.num_zones = 2,
		.zones = initproc_datazones,
	};

	// Create the initial process:
	struct process * initial_process = process_create(&initproc_memory_map);
	mmu_set_user_translation_table(initial_process->translation_table, 0);

	// Copy the initial process image:
	memcpy(PROCESS_START_ADDRESS, initproc_image, initproc_size);

	// Unmap the initial process:
	mmu_unmap(initproc_image, initproc_size);

	// Create the initial thread:
	struct thread * init_thread = process_add_new_thread(initial_process, PROCESS_START_ADDRESS, PROCESS_DEFAULT_STACK_TOP);

	// Add the new thread to the scheduler:
	scheduler_add_thread(init_thread);

	// Start the scheduler:
	scheduler_timer->start();

	while(1) asm volatile("wfi\n\t");
	return true;
}

void scheduler_add_thread(struct thread * t)
{
	debug_printf("Adding thread with PID %d, TID %d to ready queue\n", (int) t->parent->pid,
		(int) t->tid);
	queue_add_front(running_queue, t);
}

struct thread * scheduler_remove_thread(struct thread * t)
{
	debug_printf("Removing thread with PID %d, TID %d from the scheduler queues\n",
		(int) t->parent->pid, (int) t->tid);
	if(t == active_thread)
		active_thread = 0;

	// Remove the thread from the running queue:
	struct queue_node * current = running_queue->first;
	while(current != 0)
	{
		struct thread * current_thread = current->data;
		if(current_thread == t)
		{
			queue_remove_node(running_queue, current);
			break;
		}

		current = current->next;
	}

	// Remove the thread from the blocking queue:
	// TODO: think this through more closely when blocking is needed...
	current = blocking_queue->first;
	while(current != 0)
	{
		struct thread * current_thread = current->data;
		if(current_thread == t)
		{
			queue_remove_node(blocking_queue, current);
			break;
		}

		current = current->next;
	}

	return t;
}

void scheduler_move_thread_to_blocking(struct thread * t)
{
	struct queue_node * current = running_queue->first;
	while(current != 0)
	{
		struct thread * current_thread = current->data;
		if(current_thread == t)
		{
			queue_remove_node(running_queue, current);
			queue_add_back(blocking_queue, current->data);
			break;
		}

		current = current->next;
	}
}

void scheduler_move_thread_to_running(struct thread * t)
{
	struct queue_node * current = blocking_queue->first;
	while(current != 0)
	{
		struct thread * current_thread = current->data;
		if(current_thread == t)
		{
			queue_remove_node(blocking_queue, current);
			queue_add_front(running_queue, current->data);
			break;
		}
	}
}

void scheduler_reschedule()
{
	debug_printf("Rescheduling...\n");
	struct thread * next_thread;

	if(active_thread != 0 && active_thread != idle_thread)
		queue_add_back(running_queue, active_thread);

	// If no thread can be run, schedule the idle thread:
	if(!queue_remove_front(running_queue, (void **) &next_thread))
	{
		debug_printf("No thread to run, scheduling idle thread\n");
		next_thread = idle_thread;
	} else
		debug_printf("Scheduling thread with PID %d, TID %d to run\n", next_thread->parent->pid,
			next_thread->tid);

	if(active_thread != 0)
		context_copy(active_thread->context, current_context);
	context_copy(current_context, next_thread->context);

	mmu_set_user_translation_table(next_thread->parent->translation_table, next_thread->parent->pid);
	active_thread = next_thread;
}

pid_t scheduler_allocate_pid(void)
{
	pid_t retval = next_pid++;
	pid_t first_attempt = retval;

	while(rbtree_key_exists(allocated_pids, (void *) retval))
	{
		retval = next_pid++;

		// Check if all possibilities have been tried:
		if(first_attempt == retval)
			return -1;
	}

	rbtree_insert(allocated_pids, (void *) retval, (void *) true);
	return retval;
}

void scheduler_free_pid(pid_t pid)
{
	rbtree_delete(allocated_pids, (void *) pid);
}

static void idle_thread_loop(void)
{
	asm volatile(
		"1:\n\t"
		"wfi\n\t"
		"b 1b\n\t"
		:::
	);
}

