// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_SCHEDULER_H
#define MORDAX_SCHEDULER_H

#include "thread.h"
#include "api/types.h"
#include "drivers/timer/timer.h"

/**
 * @defgroup scheduler Scheduler Functionality
 * @todo Support SMP.
 * @{
 */

/** The currently active thread. */
extern struct thread * active_thread;

/** The currently active process. */
#define active_process	(active_thread != 0 ? active_thread->parent : 0)

/**
 * Initializes the scheduler.
 * @param scheduler_timer the timer to use for the periodic scheduler interrupt.
 * @param initproc_start physical address of the initial process image.
 * @param initproc_size size of the initial process image.
 * @return `true` if the initialization was successful, `false` otherwise.
 */
bool scheduler_initialize(struct timer_driver * scheduler_timer, physical_ptr initproc_start,
	size_t initproc_size);

/**
 * Adds a thread to the scheduler. If the process has not been added to the scheduler,
 * it is added as well.
 * @param t the thread to add.
 */
void scheduler_add_thread(struct thread * t);

/**
 * Removes a thread from the scheduler. This does not free the thread structure.
 * @param pid the PID of the process containing the thread.
 * @param tid the TID of the thread.
 */
struct thread * scheduler_remove_thread(struct thread * t);

/**
 * Moves a thread to the queue of blocking threads.
 * @param t the thread to move.
 */
void scheduler_move_thread_to_blocking(struct thread * t);

/**
 * Moves a thread to the queue of running threads.
 * @param t the thread to move.
 */
void scheduler_move_thread_to_running(struct thread * t);

/**
 * Forces the scheduler to do a scheduling pass.
 */
void scheduler_reschedule();

/**
 * Allocates a new process identifier (PID) for a thread.
 * @return the new process identifier, or -1 if no PID can be allocated.
 */
pid_t scheduler_allocate_pid(void);

/**
 * Frees a process identifier (PID).
 * @param pid the pid to free from the list of used PIDs.
 */
void scheduler_free_pid(pid_t pid);

/** @} */

#endif

