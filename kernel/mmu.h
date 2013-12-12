// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_MMU_H
#define MORDAX_MMU_H

#include "api/types.h"
#include "api/memory.h"

/**
 * @defgroup mmu MMU and Paging Functions
 * @{
 */

/** Translation table type. */
struct mmu_translation_table;

/** Initializes the MMU for kernel use. */
void mmu_initialize(void);

/**
 * Creates a translation table for application use.
 * @return an empty translation table for application use.
 */
struct mmu_translation_table * mmu_create_translation_table(void);

/**
 * Deletes a translation table for application use.
 * Any memory allocated using the physical memory manager is returned to the
 * free memory pool automatically. Other mapped memory is not freed in any way.
 * @param table the translation table to free.
 */
void mmu_free_translation_table(struct mmu_translation_table * table);

/**
 * Sets the current application translation table. This is used during
 * context switching to change translation tables.
 * @param table the new translation table.
 * @param pid the PID of the process.
 */
void mmu_set_user_translation_table(struct mmu_translation_table * table, pid_t pid);

/**
 * Creates a memory mapping. The address is mapped in either the userspace
 * translation table or the kernel translation table based on whether the
 * virtual address to map to is in userspace or kernelspace.
 * @param physical physical address of the memory to map. The physical address
 *                 is rounded down to a multiple of the page size.
 * @param virtual virtual address to map the memory to.
 * @param size size of the mapping to create. This is rounded up to a multiple
 *             of the page size.
 * @param type type of memory mapping
 * @param permissions memory access permissions
 */
void * mmu_map(physical_ptr physical, void * virtual, size_t size,
	enum mordax_memory_type type, enum mordax_memory_permissions permissions);

/**
 * Changes the permissions for an interval of virtual memory.
 * @param virtual the virtual address of the page to change permissions for.
 * @param size the size of the memory range to remap.
 * @param type new type for the page.
 * @param permissions new permissions for the page.
 */
void mmu_remap(void * virtual, size_t size, enum mordax_memory_type type,
	enum mordax_memory_permissions permissions);

/**
 * Maps a device for kernel use.
 * @param physical physical address of the device memory to map.
 * @param size size of the mapping to create.
 * @return the virtual address of the device mapping.
 */
void * mmu_map_device(physical_ptr physical, size_t size);

/**
 * Unmaps a section of virtual memory.
 * @param virtual the virtual address to unmap.
 * @param size the length of the mapping to unmap.
 */
void mmu_unmap(void * virtual, size_t size);

/**
 * Converts a virtual address to a physical address.
 * @param virtual the virtual address to convert.
 * @return the physical address corresponding to the specified virtual address.
 */
physical_ptr mmu_virtual_to_physical(void * virtual);

/**
 * Converts a physical address to a virtual address.
 * @param physical the physical address to convert.
 * @return the virtual address corresponding to the specified physical address.
 */
void * mmu_physical_to_virtual(physical_ptr physical);

/** @} */

#endif

