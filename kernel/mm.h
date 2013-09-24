// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_MM_H
#define MORDAX_MM_H

#include <stdbool.h>
#include <stdint.h>

#include "types.h"

/**
 * @defgroup mm_kernel Kernel Memory Management Functions
 * @{
 */

/** Flag specifying normal memory. */
#define MM_MEM_NORMAL		0

/** Default memory alignment. */
#ifndef MM_DEFAULT_ALIGNMENT
#define MM_DEFAULT_ALIGNMENT	4
#endif

/** Pointer to the end of the kernel dataspace. */
extern void * kernel_dataspace_end;

/** Initializes the memory manager. */
void mm_initialize(void);

/** Prints a list of all the memory blocks currently allocated (both used and unused). */
void print_blocks(void);

/**
 * Allocates an area of kernel memory.
 * @param size size of the area to allocate.
 * @param alignment alignment of the start of the memory area.
 * @param flags additional properties of the memory area.
 * @return the allocated memory area or `NULL` if no memory was available.
 */
void * mm_allocate(size_t size, unsigned int alignment, unsigned int flags)
	__attribute((malloc));

/**
 * Frees an area of kernel memory. Null pointers can be freed, in which case
 * no action is taken.
 * @param area the area of memory to free.
 */
void mm_free(void * area);

/**
 * @}
 */

/**
 * @defgroup mm_physical Physical Memory Management Functions
 * @{
 */

/** Zone flag specifying normal memory. */
#define MM_ZONE_NORMAL	0

/** Structure representing an area of physical memory. */
struct mm_physical_memory
{
	physical_ptr base;	/**< Base address of the memory area. */
	size_t size;		/**< Size of the memory area. */
	unsigned int flags;	/**< Properties of the memory area. */
};

/**
 * Adds a memory zone to the physical memory manager.
 * @param address physical address of the memory area.
 * @param size size of the memory area.
 * @param flags additional properties of the memory area.
 */
void mm_add_physical(physical_ptr address, size_t size, unsigned int flags);

/**
 * Allocates a chunk of physical memory.
 * @param size size of the requested memory area. Will be rounded up to a
 *             multiple of the page size.
 * @param retval structure to return the allocated physical memory region in.
 * @return `true` if memory could be allocated or `false` if no memory was available.
 */
bool mm_allocate_physical(size_t size, struct mm_physical_memory * retval);

/**
 * Frees a chunk of physical memory.
 * @param memory the physical memory area to free.
 */
void mm_free_physical(struct mm_physical_memory * memory);

/**
 * Reserves an area of physical memory from being allocated.
 * The size of the area is rounded up to the nearest multiple of the page size, while
 * the base address is rounded down. To unreserve the area, simply free it using
 * `mm_free_physical`.
 * @param address the address of the memory area to reserve.
 * @param size the length of the area to reserve.
 */
void mm_reserve_physical(physical_ptr address, size_t size);

/**
 * @}
 */

#endif

