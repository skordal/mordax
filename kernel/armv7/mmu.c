// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <stdint.h>

#include "../debug.h"
#include "../mm.h"
#include "../mmu.h"
#include "../rbtree.h"
#include "../types.h"
#include "../utils.h"

#include "mmu.h"

#define pt_index(virtual) (((uint32_t) virtual & 0xfffff) >> 12)

// The kernel translation table:
uint32_t kernel_translation_table[4096] __attribute((aligned(16*1024)));
// Kernel page table physical->virtual lookup table:
struct rbtree * kernel_lookup_table;

// Next device mapping address:
void * next_device_address = (void *) 0xf0000000;

// Maps one page of memory:
static void * mmu_map_page(physical_ptr physical, void * virtual,
	enum mmu_memory_type type, enum mmu_memory_permissions permissions);
// Unmaps one page of memory:
static void mmu_unmap_page(void * virtual);

// Gets the virtual address of the page table the specified entry in a
// translation table points to, or 0 if none:
static uint32_t * get_pt_address(uint32_t * translation_table, int index);

// Gets the type bits for the specified small page type:
static inline uint32_t small_page_type_bits(enum mmu_memory_type type);
// Gets the permission bits for the specified small page permissions:
static inline uint32_t small_page_permission_bits(enum mmu_memory_permissions permissions);

void mmu_initialize(void)
{
	extern void * text_start, * data_start, * kernel_address, * load_address;
	debug_printf("Initializing MMU...\n");

	kernel_lookup_table = rbtree_new();

	// Create the initial page table:
	uint32_t * page_table = mm_allocate(1024, 1024, MM_MEM_NORMAL);
	memclr(page_table, 1024);

	// Map the kernel code section:
	uint32_t text_size = ((uint32_t) &data_start - (uint32_t) &text_start + 4095) & -4096;
	for(unsigned i = 0; i < text_size >> 12; ++i)
	{
		uint32_t virtual = (uint32_t) &kernel_address + (i << 12);
		uint32_t entry = ((uint32_t) mmu_virtual_to_physical((void *) virtual))
			| MMU_SMALL_PAGE_TYPE | MMU_SMALL_PAGE_CODE | MMU_SMALL_PAGE_RO_NA;
		page_table[pt_index(virtual)] = entry;
	}

	// Map the kernel data section:
	uint32_t data_size = ((uint32_t) kernel_dataspace_end - (uint32_t) &data_start + 4095) & -4096;
	for(unsigned i = 0; i < data_size >> 12; ++i)
	{
		uint32_t virtual = (uint32_t) &data_start + (i << 12);
		uint32_t entry = ((uint32_t) mmu_virtual_to_physical((void *) virtual))
			| MMU_SMALL_PAGE_TYPE | MMU_SMALL_PAGE_DATA | MMU_SMALL_PAGE_RW_NA;
		page_table[pt_index(virtual)] = entry;
	}

	// Add the page table to the kernel translation table:
	uint32_t pt_entry = (uint32_t) mmu_virtual_to_physical(page_table) | MMU_PAGE_TABLE_TYPE;
	kernel_translation_table[(uint32_t) &kernel_address >> 20] = pt_entry;
	rbtree_insert(kernel_lookup_table, (uint32_t) mmu_virtual_to_physical(page_table), (void *) page_table);

	// Clear temporary section mappings:
	uint32_t old_mapping_size = ((uint32_t) &kernel_dataspace_end - (uint32_t) &kernel_address + (1024 * 1024)) & -(1024 * 1024);
	for(unsigned i = 0; i < old_mapping_size >> 20; ++i)
		kernel_translation_table[((uint32_t) &load_address >> 20) + i] = 0;

	// Finally clear the TLB so that all new mappings take effect:
	asm volatile("mcr p15, 0, r0, c8, c7, 0\n\tisb\n\tdsb\n\t" ::: "memory");
}

void * mmu_map(physical_ptr physical, void * virtual, size_t size,
	enum mmu_memory_type type, enum mmu_memory_permissions permissions)
{
	size = (size + 4095) & -4096;
	debug_printf("Going to map %p (size: %d) to %p\n", physical, size, virtual);

	for(unsigned i = 0; i < size >> 12; ++i)
	{
		mmu_map_page((physical_ptr) ((uint32_t) physical + (i << 12)),
			(void *) ((uint32_t) virtual + (i << 12)), type, permissions);
	}

	return virtual;
}

void * mmu_map_device(physical_ptr physical, size_t size)
{
	void * retval = next_device_address;
	size = (size + 4095) & -4096;
	next_device_address = (void *) ((uint32_t) next_device_address + size);
	mmu_map(physical, retval, size, MMU_TYPE_DEVICE, MMU_PERM_RW_NA);
	return retval;
}

void mmu_unmap(void * virtual, size_t size)
{
	size = (size + 4095) & -4096;
	debug_printf("Unmapping %p (length: %x)\n", virtual, size);

	for(int i = 0; i < size >> 12; ++i)
		mmu_unmap_page((void *) ((uint32_t) virtual + (i << 12)));
}

physical_ptr mmu_virtual_to_physical(void * virtual)
{
	uint32_t retval, offset = (uint32_t) virtual & 0xfff;

	// The resolution of the MMU translation instruction is 4096 bytes, so
	// clear the unused bits:
	virtual = (void *) ((uint32_t) virtual & 0xfffff000);

	asm volatile(
		"mcr p15, 0, %[virtual], c7, c8, 0\n\t"
		"isb\n\t"
		"mrc p15, 0, %[retval], c7, c4, 0\n\t"
		: [retval] "=r" (retval)
		: [virtual] "r" (virtual)
		:
	);

	return (void *) ((retval & 0xfffff000) + offset);
}

void * mmu_physical_to_virtual(physical_ptr physical)
{
	return rbtree_get_value(kernel_lookup_table, (uint32_t) physical); // FIXME: kernel/user table
}

static void * mmu_map_page(physical_ptr physical, void * virtual,
	enum mmu_memory_type type, enum mmu_memory_permissions permissions)
{
//	uint32_t * table = (uint32_t) virtual >= MMU_KERNEL_SPLIT_ADDRESS ?
//		kernel_translation_table : user_translation_table;
	uint32_t * table = kernel_translation_table;
	uint32_t * page_table = get_pt_address(table, (uint32_t) virtual >> 20);

	// Round the physical (and virtual) addresses down:
	physical = (void *) ((uint32_t) physical & -4096);
	virtual = (void *) ((uint32_t) virtual & -4096);

	if(page_table == 0)
	{
		// Allocate a new page table:
		page_table = mm_allocate(1024, 1024, MM_MEM_NORMAL);
		memclr(page_table, 1024);
		rbtree_insert(kernel_lookup_table, (uint32_t) mmu_virtual_to_physical(page_table),
			page_table); // FIXME: kernel/user table
		table[(uint32_t) virtual >> 20] = (uint32_t) mmu_virtual_to_physical(page_table)
			| MMU_PAGE_TABLE_TYPE;
	}

	uint32_t entry = ((uint32_t) physical & MMU_SMALL_PAGE_BASE_MASK) | MMU_SMALL_PAGE_TYPE |
		small_page_type_bits(type) | small_page_permission_bits(permissions);
	page_table[((uint32_t) virtual & 0xfffff) >> 12] = entry;

	// Insert the page into the lookup table:
	rbtree_insert(kernel_lookup_table, (uint32_t) physical, virtual); // FIXME: kernel/user table

	return virtual;
}

static void mmu_unmap_page(void * virtual)
{
	uint32_t * table = kernel_translation_table;
	uint32_t * page_table = get_pt_address(table, (uint32_t) virtual >> 20);

	// Round the address down:
	virtual = (void *) ((uint32_t) virtual & -4096);
	rbtree_remove(kernel_lookup_table, (uint32_t) mmu_virtual_to_physical(virtual)); // FIXME: kernel/user table

	if(page_table != 0)
		page_table[((uint32_t) virtual & 0xfffff) >> 12] = 0;
	else
		debug_printf("Cannot unmap memory: no existing entry\n");
}

static uint32_t * get_pt_address(uint32_t * translation_table, int index)
{
	if((translation_table[index] & 0x3) != MMU_PAGE_TABLE_TYPE)
		return 0;
	else
		return mmu_physical_to_virtual((void *) (translation_table[index] & MMU_PAGE_TABLE_BASE_MASK));
}

static inline uint32_t small_page_type_bits(enum mmu_memory_type type)
{
	switch(type)
	{
		case MMU_TYPE_CODE:
			return MMU_SMALL_PAGE_CODE;
		case MMU_TYPE_RODATA:
			return MMU_SMALL_PAGE_RODATA;
		case MMU_TYPE_DATA:
			return MMU_SMALL_PAGE_DATA;
		case MMU_TYPE_STACK:
			return MMU_SMALL_PAGE_STACK;
		case MMU_TYPE_STRORD:
			return MMU_SMALL_PAGE_STRORD;
		case MMU_TYPE_DEVICE:
			return MMU_SMALL_PAGE_DEVICE;
		case MMU_TYPE_UNCACHED:
			return MMU_SMALL_PAGE_UNCACHED;
		default:
			return 0;
	}
}

static inline uint32_t small_page_permission_bits(enum mmu_memory_permissions permissions)
{
	switch(permissions)
	{
		case MMU_PERM_RW_RW:
			return MMU_SMALL_PAGE_RW_RW;
		case MMU_PERM_RW_RO:
			return MMU_SMALL_PAGE_RW_RO;
		case MMU_PERM_RW_NA:
			return MMU_SMALL_PAGE_RW_NA;
		case MMU_PERM_RO_RO:
			return MMU_SMALL_PAGE_RO_RO;
		case MMU_PERM_RO_NA:
			return MMU_SMALL_PAGE_RO_NA;
		case MMU_PERM_NA_NA:
			return MMU_SMALL_PAGE_NA_NA;
		default:
			return 0;
	}
}

