// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "api/memory.h"
#include "api/process.h"

#include "context.h"
#include "debug.h"
#include "kernel.h"
#include "number_allocator.h"
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

// PID allocator object:
static struct number_allocator * pid_allocator;

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
	scheduler_timer->set_interval(100000);
	scheduler_timer->set_callback(scheduler_reschedule);

	running_queue = queue_new();
	blocking_queue = queue_new();
	pid_allocator = number_allocator_new();

	// Create the idle process + thread:
	debug_printf("Creating idle process...\n");
	struct mordax_process_info idle_process_info = {
		.entry_point = (void *) idle_thread_loop,
		.permissions = MORDAX_PROCESS_NO_PERMISSIONS
	};
	struct process * idle_process = process_create(&idle_process_info);
	if(idle_process == 0)
		kernel_panic("could not create idle process");
	idle_thread = process_add_new_thread(idle_process, (void *) idle_thread_loop, 0);
	if(idle_thread == 0)
		kernel_panic("could not create idle thread");
	context_set_mode(idle_thread->context, CONTEXT_KERNELMODE);

	// Map the initial process:
	debug_printf("Creating initial process...\n");
	uint8_t * initproc_image = mmu_map(0, initproc_start, initproc_start, initproc_size,
		MORDAX_TYPE_DATA, MORDAX_PERM_RO_RO);

	// Create the initial process:
	struct mordax_process_info initproc_info = {
		.entry_point = (void *) 0x1000,
		.stack_length = PROCESS_DEFAULT_STACK_SIZE,
		.permissions = MORDAX_PROCESS_ALL_PERMISSIONS,
		.source = initproc_image,
		.source_length = initproc_size,
		.text_length = (initproc_size + CONFIG_PAGE_SIZE - 1) & -CONFIG_PAGE_SIZE,
	};
	struct process * initial_process = process_create(&initproc_info);
	if(initial_process == 0)
		kernel_panic("could not create the initial process");

	// Unmap the initial process:
	mmu_unmap(0, initproc_image, initproc_size);

	// Create the initial thread:
	struct thread * init_thread = process_add_new_thread(initial_process, (void *) PROCESS_START_ADDRESS,
		(void *) PROCESS_DEFAULT_STACK_TOP);

	// Add the new thread to the scheduler:
	scheduler_add_thread(init_thread);

	// Start the scheduler:
	scheduler_timer->start();

	while(1) asm volatile("wfi\n\t");
	return true;
}

void scheduler_add_thread(struct thread * t)
{
	queue_add_front(running_queue, t);
}

struct thread * scheduler_remove_thread(struct thread * t)
{
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

	return t;
}

void scheduler_move_thread_to_blocking(struct thread * t)
{
	struct queue_node * current = running_queue->first;

	if(t == active_thread)
	{
		context_copy(t->context, current_context);
		queue_add_back(blocking_queue, t);
		active_thread = 0;
	} else while(current != 0)
	{
		struct thread * current_thread = current->data;
		if(current_thread == t)
		{
			queue_remove_node(running_queue, current);
			queue_add_back(blocking_queue, current_thread);
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
	struct thread * next_thread;

	if(active_thread != 0 && active_thread != idle_thread)
		queue_add_back(running_queue, active_thread);

	// If no thread can be run, schedule the idle thread:
	if(!queue_remove_front(running_queue, (void **) &next_thread))
		next_thread = idle_thread;

	if(active_thread != 0)
		context_copy(active_thread->context, current_context);
	context_copy(current_context, next_thread->context);

	if(next_thread == idle_thread)
		mmu_set_translation_table(0);
	else
		mmu_set_translation_table(next_thread->parent->translation_table);
	active_thread = next_thread;
}

pid_t scheduler_allocate_pid(void)
{
	return number_allocator_allocate_num(pid_allocator) - 1;
}

void scheduler_free_pid(pid_t pid)
{
	number_allocator_free_num(pid_allocator, pid + 1);
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

