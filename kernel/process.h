// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_PROCESS_H
#define MORDAX_PROCESS_H

#include "mmu.h"
#include "queue.h"
#include "rbtree.h"

#include "api/memory.h"
#include "api/types.h"

#ifndef CONFIG_DEFAULT_STACK_SIZE
#error "user-space stack size not set, define CONFIG_DEFAULT_STACK_SIZE with the proper stack size"
#endif

#ifndef CONFIG_DEFAULT_STACK_BASE
#error "user-space base address not set, define CONFIG_DEFAULT_STACK_BASE with the proper stack base"
#endif

/**
 * @defgroup process Process Support
 * @{
 */

struct thread;

/** Default stack size. */
#define PROCESS_DEFAULT_STACK_SIZE	CONFIG_DEFAULT_STACK_SIZE
/** Default stack base address. */
#define PROCESS_DEFAULT_STACK_BASE	(void *) CONFIG_DEFAULT_STACK_BASE
/** Top of the default stack. */
#define PROCESS_DEFAULT_STACK_TOP	(void *) (CONFIG_DEFAULT_STACK_BASE + CONFIG_DEFAULT_STACK_SIZE)

/**
 * Default process start address. One page at address 0 is reserved in order
 * for programmes to crash when attempting to dereference null pointers.
 */
#define PROCESS_START_ADDRESS	(void *) CONFIG_PAGE_SIZE

struct process
{
	struct mmu_translation_table * translation_table;
	struct queue * threads;
	unsigned int num_threads;
	pid_t pid;

	struct rbtree * allocated_tids;
	tid_t next_tid;

};

/**
 * Creates a new process with the specified memory layout.
 * Memory is allocated and mapped according to the initial memory map.
 * @param memory_map a memory map containing the initial layout of the process.
 * @return the allocated process.
 */
struct process * process_create(struct mordax_memory_map * memory_map);

/**
 * Frees a process.
 * @param p the process to free.
 */
void process_free(struct process * p);

/**
 * Adds a thread to a process.
 * @param p the process.
 * @param t the thread.
 */
void process_add_thread(struct process * p, struct thread * t);

/**
 * Creates and adds a thread to a process.
 * @param p the process.
 * @param entry thread entry-point address.
 * @param stack pointer to the top of the stack for the thread.
 * @return a pointer to the thread that was created.
 */
struct thread * process_add_new_thread(struct process * p, void * entry, void * stack);

/**
 * Removes a thread from a process.
 * @param p the process.
 * @param t the thread.
 * @return a pointer to the removed thread.
 */
struct thread * process_remove_thread(struct process * p, struct thread * t);

/**
 * Looks up a thread by its TID.
 * @param p the process.
 * @param tid the TID to look up.
 * @return a pointer to the thread with the specified TID or 0 if the thread was not found.
 */
struct thread * process_get_thread_by_tid(struct process * p, tid_t tid);

/** @} */

#endif

