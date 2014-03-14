// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_PROCESS_H
#define MORDAX_PROCESS_H

#include "mmu.h"
#include "number_allocator.h"
#include "queue.h"
#include "rbtree.h"
#include "service.h"
#include "socket.h"

#include "api/memory.h"
#include "api/process.h"
#include "api/types.h"

#ifndef CONFIG_DEFAULT_STACK_SIZE
#error "user-space stack size not set, define CONFIG_DEFAULT_STACK_SIZE with the proper stack size"
#endif

#ifndef CONFIG_DEFAULT_STACK_BASE
#error "user-space base address not set, define CONFIG_DEFAULT_STACK_BASE with the proper stack base"
#endif

/** Default stack size. */
#define PROCESS_DEFAULT_STACK_SIZE	CONFIG_DEFAULT_STACK_SIZE
/** Default stack base address. */
#define PROCESS_DEFAULT_STACK_BASE	CONFIG_DEFAULT_STACK_BASE
/** Top of the default stack. */
#define PROCESS_DEFAULT_STACK_TOP	(CONFIG_DEFAULT_STACK_BASE + CONFIG_DEFAULT_STACK_SIZE)

/**
 * @defgroup process Process Support
 * @{
 */

/**
 * Default process start address. One page at address 0 is reserved in order
 * for programmes to crash when attempting to dereference null pointers.
 */
#define PROCESS_START_ADDRESS	(void *) CONFIG_PAGE_SIZE

struct thread;

struct process
{
	struct mmu_translation_table * translation_table;
	struct queue * threads;
	unsigned int num_threads;
	pid_t pid;

	struct rbtree * allocated_tids;
	tid_t next_tid;

	struct rbtree * resource_table;
	struct number_allocator * resnum_allocator;

	uint32_t permissions;
	gid_t owner_group;
	uid_t owner_user;

	size_t stack_size;
};

enum process_resource_type
{
	PROCESS_RESOURCE_SOCKET,
	PROCESS_RESOURCE_SERVICE,
	PROCESS_RESOURCE_LOCK,
	PROCESS_RESOURCE_DT_NODE
};

/**
 * Creates a new process with the specified memory layout.
 * Memory is allocated and mapped according to the initial memory map.
 * @param memory_map a memory map containing the initial layout of the process.
 * @param uid ID of the user owning the process.
 * @param gid ID of the group owning the process.
 * @return the allocated process.
 */
struct process * process_create(struct mordax_process_info * procinfo)
	__attribute((malloc));

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
 * Adds a resource to a process.
 * @param p the process.
 * @param type the resource type.
 * @param resource_ptr pointer to the resource structure.
 * @return a numeric handle to the resource, which can be returned by a syscall.
 */
unsigned int process_add_resource(struct process * p, enum process_resource_type type,
	void * resource_ptr);

/**
 * Gets a resource by its identifier.
 * @param p the process.
 * @param identifier resource identifier.
 * @param type the type of the returned resource is stored in the variable
 *             pointed to by this parameter.
 * @return a pointer to the process resource structure associated with the
 *         structure.
 */
void * process_get_resource(struct process * p, unsigned int identifier,
	enum process_resource_type * type);

/**
 * Removes a process resource.
 * @param p the process.
 * @param identifier resource identifier.
 * @param type the type of the returned resource is stored in the variable
 *             pointed to by this parameter.
 * @return the removed resource.
 */
void * process_remove_resource(struct process * p, unsigned int identifier,
	enum process_resource_type * type);

/**
 * Looks up a thread by its TID.
 * @param p the process.
 * @param tid the TID to look up.
 * @return a pointer to the thread with the specified TID or 0 if the thread was not found.
 */
struct thread * process_get_thread_by_tid(struct process * p, tid_t tid);

/** @} */

#endif

