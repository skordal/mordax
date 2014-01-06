// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <stdint.h>

#include "../api/types.h"

#include "../debug.h"
#include "../mm.h"
#include "../mmu.h"
#include "../rbtree.h"
#include "../utils.h"

#include "mmu.h"

#define pt_index(virtual) (((uint32_t) virtual & 0xfffff) >> 12)

// Structure used for user translation tables:
struct mmu_translation_table
{
	uint32_t table[2048];
	struct rbtree * lookup_table;
	pid_t pid;
};

// Structure used in lookup tables:
struct lookup_table_entry
{
	void * virtual, * physical;
	enum { PT_ADDRESS, MEM_ADDRESS } type;
};

// The kernel translation table:
uint32_t kernel_translation_table[4096] __attribute((aligned(16*1024)));
// Kernel page table physical->virtual lookup table:
struct rbtree * kernel_lookup_table;

// The current application translation table:
static struct mmu_translation_table * user_translation_table;

// Next device mapping address:
static void * next_device_address = (void *) 0xf0000000;

// Stack for allocating page tables:
static struct mm_object_stack * pt_stack;

// Function called for freeing an entry in a lookup table:
static void free_lookup_entry(struct lookup_table_entry *);

// Maps one page of memory:
static void * mmu_map_page(struct mmu_translation_table * t, physical_ptr physical, void * virtual,
	enum mordax_memory_type type, enum mordax_memory_permissions permissions);
// Unmaps one page of memory:
static void mmu_unmap_page(struct mmu_translation_table * t, void * virtual);
// Changes the attributes for one page of memory:
static void mmu_change_page_attributes(struct mmu_translation_table * t, void * virtual,
	enum mordax_memory_type type, enum mordax_memory_permissions permissions);

// Gets the virtual address of the page table the specified entry in a
// translation table points to, or 0 if none:
static uint32_t * get_pt_address(uint32_t * translation_table, int index);

// Creates a lookup table entry for a page table:
static inline struct lookup_table_entry * lookup_entry_pt(void * virtual);
// Creates a lookup table entry for a memory page:
static inline struct lookup_table_entry * lookup_entry_mem(physical_ptr physical, void * virtual);

// Gets the type bits for the specified small page type:
static inline uint32_t small_page_type_bits(enum mordax_memory_type type);
// Gets the permission bits for the specified small page permissions:
static inline uint32_t small_page_permission_bits(enum mordax_memory_permissions permissions);

// ASID counter:
static uint8_t asid = 0;

void mmu_initialize(void)
{
	extern void * text_start, * data_start, * kernel_address, * load_address;
	kernel_lookup_table = rbtree_new(0, 0, 0, mm_free);

	// Create the page table stack:
	pt_stack = mm_object_stack_create(1024, 1024, 16, 16);

	// Create the initial page table:
	uint32_t * page_table = mm_object_stack_allocate(pt_stack);
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
	rbtree_insert(kernel_lookup_table, mmu_virtual_to_physical(page_table), lookup_entry_pt(page_table));

	// Clear temporary section mappings:
	uint32_t old_mapping_size = ((uint32_t) &kernel_dataspace_end - (uint32_t) &kernel_address + (1024 * 1024)) & -(1024 * 1024);
	for(unsigned i = 0; i < old_mapping_size >> 20; ++i)
		kernel_translation_table[((uint32_t) &load_address >> 20) + i] = 0;

	// Finish MMU initialization with some assembler code:
	asm volatile(
		// Set domain permissions:
		"ldr ip, =0x55555555\n\t"
		"mcr p15, 0, ip, c3, c0, 0\n\t"
		// Set the TTBCR.N to 1 to use 8196 byte translation tables for userspace mappings:
		"mov ip, #1\n\t"
		"mcr p15, 0, ip, c2, c0, 2\n\t"
		// Finally, clear the TLB so that all new mappings take effect:
		"mcr p15, 0, ip, c8, c7, 0\n\t"
		"isb\n\t"
		"dsb\n\r"
		::: "ip", "memory"
	);
}

struct mmu_translation_table * mmu_create_translation_table(pid_t pid)
{
	struct mmu_translation_table * retval = mm_allocate(sizeof(struct mmu_translation_table), 8192, MM_MEM_NORMAL);

	memclr(retval, sizeof(struct mmu_translation_table));
	retval->lookup_table = rbtree_new(0, 0, 0, (rbtree_data_free_func) free_lookup_entry);
	retval->pid = pid;

	return retval;
}

void mmu_free_translation_table(struct mmu_translation_table * table)
{
	rbtree_free(table->lookup_table);
	mm_free(table);
}

static void free_lookup_entry(struct lookup_table_entry * entry)
{
	if(entry->type == PT_ADDRESS)
		mm_object_stack_free(pt_stack, entry->virtual);
	else if(entry->type == MEM_ADDRESS)
	{
		struct mm_physical_memory memory = { .base = entry->physical, .size = CONFIG_PAGE_SIZE };
		if(mm_is_physical_managed(memory.base))
			mm_free_physical(&memory);
	}

	mm_free(entry);
}

void mmu_set_translation_table(struct mmu_translation_table * table)
{
	user_translation_table = table;
	void * table_physical = mmu_virtual_to_physical(user_translation_table->table);
	++asid;

	// Switch TTBR0 to point to the new translation table:
	asm volatile(
		// Set the TTBCR.PD0 bit to 1, disabling translation using TTBR0:
		"mrc p15, 0, ip, c2, c0, 2\n\t"
		"orr ip, #(1 << 4)\n\t"
		"mcr p15, 0, ip, c2, c0, 2\n\r"
		"isb\n\r"

		// Change the CONTEXTIDR:
		"lsl ip, %[pid], #8\n\t"
		"orr ip, %[asid]\n\t"
		"mcr p15, 0, ip, c13, c0, 1\n\t"

		// Change the TTBR0 register:
		"mcr p15, 0, %[table], c2, c0, 0\n\t"
		"isb\n\t"

		// Clear the TTBCR.PD0 bit:
		"mrc p15, 0, ip, c2, c0, 2\n\t"
		"mvn v1, #(1 << 4)\n\t"
		"and ip, v1\n\t"
		"mcr p15, 0, ip, c2, c0, 2\n\t"
		"dsb\n\t"
		"isb\n\t"
		:
		: [asid] "r" (asid), [pid] "r" (table->pid), [table] "r" (table_physical)
		: "ip", "v1"
	);

	if(asid == 0)
		asm volatile("mcr p15, 0, ip, c8, c7, 0\n\tisb\n\tdsb\n\t"); // clear the TLB
}

struct mmu_translation_table * mmu_get_translation_table(void)
{
	return user_translation_table;
}

bool mmu_access_permitted(struct mmu_translation_table * t, void * address, size_t size, int flags)
{
	// TODO: implement access permission checking.
	return true;
}

void * mmu_map(struct mmu_translation_table * t, physical_ptr physical, void * virtual, size_t size,
	enum mordax_memory_type type, enum mordax_memory_permissions permissions)
{
	size = (size + 4095) & -4096;
	for(unsigned i = 0; i < size >> 12; ++i)
	{
		mmu_map_page(t, (physical_ptr) ((uint32_t) physical + (i << 12)),
			(void *) ((uint32_t) virtual + (i << 12)), type, permissions);
	}

	return virtual;
}

void mmu_change_attributes(struct mmu_translation_table * t, void * virtual, size_t size,
	enum mordax_memory_type type, enum mordax_memory_permissions permissions)
{
	size = (size + 4095) & -4096;
	for(unsigned i = 0; i < size >> 12; ++i)
		mmu_change_page_attributes(t, (void *) ((uint32_t) virtual + (i << 12)),
			type, permissions);
}

static void mmu_change_page_attributes(struct mmu_translation_table * t, void * virtual,
	enum mordax_memory_type type, enum mordax_memory_permissions permissions)
{
	virtual = (void *) ((uint32_t) virtual & -4096);

	uint32_t * table;
	if((uint32_t) virtual >= MMU_KERNEL_SPLIT_ADDRESS || t == 0)
		table = kernel_translation_table;
	else
		table = t->table;
	uint32_t * page_table = get_pt_address(table, (uint32_t) virtual >> 20);

	if(page_table == 0) // If there is no page table, return.
		return;
	if((page_table[((uint32_t) virtual & 0xfffff) >> 12] & 3) == 0) // If there is no entry, return.
		return;

	uint32_t physical = page_table[((uint32_t) virtual & 0xfffff) >> 12] & MMU_SMALL_PAGE_BASE_MASK;
	debug_printf("Altering mapping of %p -> %p...\n", (void *) physical, virtual);

	uint32_t entry = physical | MMU_SMALL_PAGE_TYPE |
		small_page_type_bits(type) | small_page_permission_bits(permissions);
	if((uint32_t) virtual < MMU_KERNEL_SPLIT_ADDRESS)
		entry |= 1 << MMU_SMALL_PAGE_NG; // Set the not-global bit for userspace pages.
	page_table[((uint32_t) virtual & 0xfffff) >> 12] = entry;
}

void * mmu_map_device(physical_ptr physical, size_t size)
{
	void * retval = next_device_address;
	size = (size + 4095) & -4096;
	next_device_address = (void *) ((uint32_t) next_device_address + size);
	mmu_map(0, physical, retval, size, MORDAX_TYPE_DEVICE, MORDAX_PERM_RW_NA);
	return retval;
}

void mmu_unmap(struct mmu_translation_table * t, void * virtual, size_t size)
{
	size = (size + 4095) & -4096;
	for(int i = 0; i < size >> 12; ++i)
		mmu_unmap_page(t, (void *) ((uint32_t) virtual + (i << 12)));
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
	if(rbtree_key_exists(kernel_lookup_table, physical))
		return ((struct lookup_table_entry *) rbtree_get_value(kernel_lookup_table, physical))->virtual;
	else
		return ((struct lookup_table_entry *) rbtree_get_value(user_translation_table->lookup_table, physical))->virtual;
}

static void * mmu_map_page(struct mmu_translation_table * t, physical_ptr physical, void * virtual,
	enum mordax_memory_type type, enum mordax_memory_permissions permissions)
{
	uint32_t * table;
	if((uint32_t) virtual >= MMU_KERNEL_SPLIT_ADDRESS || t == 0)
		table = kernel_translation_table;
	else
		table = t->table;
	uint32_t * page_table = get_pt_address(table, (uint32_t) virtual >> 20);

	// Round the physical (and virtual) addresses down:
	physical = (void *) ((uint32_t) physical & -4096);
	virtual = (void *) ((uint32_t) virtual & -4096);

	if(page_table == 0)
	{
		// Allocate a new page table:
		page_table = mm_object_stack_allocate(pt_stack);
		memclr(page_table, 1024);
		if(table == kernel_translation_table)
			rbtree_insert(kernel_lookup_table, mmu_virtual_to_physical(page_table), lookup_entry_pt(page_table));
		else
			rbtree_insert(t->lookup_table, mmu_virtual_to_physical(page_table), lookup_entry_pt(page_table));
		table[(uint32_t) virtual >> 20] = (uint32_t) mmu_virtual_to_physical(page_table)
			| MMU_PAGE_TABLE_TYPE;
	}

	uint32_t entry = ((uint32_t) physical & MMU_SMALL_PAGE_BASE_MASK) | MMU_SMALL_PAGE_TYPE |
		small_page_type_bits(type) | small_page_permission_bits(permissions);
	if((uint32_t) virtual < MMU_KERNEL_SPLIT_ADDRESS)
		entry |= 1 << MMU_SMALL_PAGE_NG; // Set the not-global bit for userspace pages.

	page_table[((uint32_t) virtual & 0xfffff) >> 12] = entry;

	// Insert the page into the lookup table:
	if(table == kernel_translation_table)
		rbtree_insert(kernel_lookup_table, physical, lookup_entry_mem(physical, virtual));
	else
		rbtree_insert(t->lookup_table, physical, lookup_entry_mem(physical, virtual));

	return virtual;
}

static void mmu_unmap_page(struct mmu_translation_table * t, void * virtual)
{
	uint32_t * table;
	if((uint32_t) virtual >= MMU_KERNEL_SPLIT_ADDRESS || t == 0)
		table = kernel_translation_table;
	else
		table = t->table;

	uint32_t * page_table = get_pt_address(table, (uint32_t) virtual >> 20);

	// Round the address down:
	virtual = (void *) ((uint32_t) virtual & -4096);
	if(table == kernel_translation_table)
		rbtree_delete(kernel_lookup_table, mmu_virtual_to_physical(virtual));
	else
		rbtree_delete(t->lookup_table, mmu_virtual_to_physical(virtual));

	if(page_table != 0)
		page_table[((uint32_t) virtual & 0xfffff) >> 12] = 0;
	else
		debug_printf("Cannot unmap memory: no existing entry\n");
}

void mmu_invalidate(void)
{
	asm volatile("mcr p15, 0, ip, c8, c7, 0\n\tisb\n\tdsb\n\t"); // clear the TLB
}

static uint32_t * get_pt_address(uint32_t * translation_table, int index)
{
	if((translation_table[index] & 0x3) != MMU_PAGE_TABLE_TYPE)
		return 0;
	else
		return mmu_physical_to_virtual((void *) (translation_table[index] & MMU_PAGE_TABLE_BASE_MASK));
}

static inline struct lookup_table_entry * lookup_entry_pt(void * virtual)
{
	struct lookup_table_entry * retval = mm_allocate(sizeof(struct lookup_table_entry),
		MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	retval->virtual = virtual;
	retval->type = PT_ADDRESS;
	return retval;
}

static inline struct lookup_table_entry * lookup_entry_mem(physical_ptr physical, void * virtual)
{
	struct lookup_table_entry * retval = mm_allocate(sizeof(struct lookup_table_entry),
		MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	retval->virtual = virtual;
	retval->physical = physical;
	retval->type = MEM_ADDRESS;
	return retval;
}

static inline uint32_t small_page_type_bits(enum mordax_memory_type type)
{
	switch(type)
	{
		case MORDAX_TYPE_CODE:
			return MMU_SMALL_PAGE_CODE;
		case MORDAX_TYPE_RODATA:
			return MMU_SMALL_PAGE_RODATA;
		case MORDAX_TYPE_DATA:
			return MMU_SMALL_PAGE_DATA;
		case MORDAX_TYPE_STACK:
			return MMU_SMALL_PAGE_STACK;
		case MORDAX_TYPE_STRORD:
			return MMU_SMALL_PAGE_STRORD;
		case MORDAX_TYPE_DEVICE:
			return MMU_SMALL_PAGE_DEVICE;
		case MORDAX_TYPE_UNCACHED:
			return MMU_SMALL_PAGE_UNCACHED;
		default:
			return 0;
	}
}

static inline uint32_t small_page_permission_bits(enum mordax_memory_permissions permissions)
{
	switch(permissions)
	{
		case MORDAX_PERM_RW_RW:
			return MMU_SMALL_PAGE_RW_RW;
		case MORDAX_PERM_RW_RO:
			return MMU_SMALL_PAGE_RW_RO;
		case MORDAX_PERM_RW_NA:
			return MMU_SMALL_PAGE_RW_NA;
		case MORDAX_PERM_RO_RO:
			return MMU_SMALL_PAGE_RO_RO;
		case MORDAX_PERM_RO_NA:
			return MMU_SMALL_PAGE_RO_NA;
		case MORDAX_PERM_NA_NA:
			return MMU_SMALL_PAGE_NA_NA;
		default:
			return 0;
	}
}

