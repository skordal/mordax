// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_PROCESS_H
#define MORDAX_PROCESS_H

#include "mmu.h"
#include "queue.h"
#include "thread.h"
#include "types.h"

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
	unsigned int refcount; //< How many threads is part of this process.
};

struct process_memory_zone
{
	void * base;
	size_t size;
	enum mmu_memory_type type;
	enum mmu_memory_permissions permissions;
};

struct process_memory_map
{
	int num_zones;
	struct process_memory_zone * zones;
};

/**
 * Creates a new process with the specified memory layout.
 * Memory is allocated and mapped according to the initial memory map.
 * @param memory_map a memory map containing the initial layout of the process.
 * @return the allocated process.
 */
struct process * process_create(struct process_memory_map * memory_map);

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

/** @} */

#endif

