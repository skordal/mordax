// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "mm.h"
#include "utils.h"

#ifndef CONFIG_PAGE_SIZE
#error "page size not set, define CONFIG_PAGE_SIZE with the proper page size"
#endif

#ifndef CONFIG_BUDDY_MAX_ORDER
#error "buddy allocator max order not set, define CONFIG_BUDDY_MAX_ORDER with the proper value"
#endif

#ifndef CONFIG_MIN_FREE_MEM
#error "minimum free memory amount not set, define CONFIG_MIN_FREE_MEM with the proper value"
#endif

/** Minimum memory block size. */
#ifndef MINIMUM_BLOCK_SIZE
#define MINIMUM_BLOCK_SIZE	MM_DEFAULT_ALIGNMENT
#endif

// Virtual memory block:
struct memory_block
{
	size_t size;
	void * start;
	bool used;
	struct memory_block * next, * prev;
};

// Physical memory zone:
struct memory_zone
{
	size_t size;
	unsigned int flags;
	physical_ptr start;
	struct memory_zone * next;
	uint8_t ** buddy_lists;
};

// Imported from the linker script:
extern void * bss_end;

// Pointer to the end of the mapped kernel dataspace, may be updated by the
// platform initialization code:
void * kernel_dataspace_end = &bss_end;

// Head of the list of memory blocks:
struct memory_block * memory_list = (void *) &bss_end;

// Head of the list of physical memory zones:
struct memory_zone * physical_memory_list = 0;

// Total amount of free memory:
static size_t total_free_memory = 0;

// Splits a block at the specified offset:
static struct memory_block * mm_split(struct memory_block *, unsigned offset);

// Allocates a physical memory block of the specified order in the specified zone:
static physical_ptr mm_allocate_order(struct memory_zone * zone, unsigned order);
// Helper function for freeing a block with the specified bit number in the buddy list
// for the specified order. This function takes care of coalescing blocks when possible.
static void mm_free_order(struct memory_zone * zone, unsigned order, unsigned bitnum);

// Calculates the order of physical block needed to allocate the specified amount
// of memory:
static inline unsigned size_to_order(size_t size);
// Calculates the number of bits required to represent a memory area of size
// at the specified order in the buddy lists:
static inline unsigned order_bits(unsigned order, size_t size);
// Calculates the size of a physical memory block of the specified order:
static inline size_t order_blocksize(unsigned order);

void print_blocks(void)
{
	struct memory_block * current= memory_list;
	while(current != 0)
	{
		debug_printf("Block @ %p\n", current);
		debug_printf("\tstart: %p\n", current->start);
		debug_printf("\tnext: %p\n", current->next);
		debug_printf("\tprev: %p\n", current->prev);
		debug_printf("\tsize: %d\n", (unsigned int) current->size);
		debug_printf("\tused: %s\n", current->used ? "true" : "false");

		current = current->next;
	}
}

void mm_initialize(void)
{
	struct memory_block * first_block = memory_list;
	memclr(first_block, sizeof(struct memory_block));
	first_block->start = (void *) ((uint32_t) memory_list + sizeof(struct memory_block));
	first_block->size = ((uint32_t) kernel_dataspace_end - (uint32_t) &bss_end - sizeof(struct memory_block));
	total_free_memory = first_block->size;
}

void * mm_allocate(size_t size, unsigned int alignment, unsigned int flags)
{
	void * retval = 0;

	if(alignment < MM_DEFAULT_ALIGNMENT)
		alignment = MM_DEFAULT_ALIGNMENT;

	if(size <= MINIMUM_BLOCK_SIZE)
		size = MINIMUM_BLOCK_SIZE;
	size += 3;
	size &= -4;

	struct memory_block * current = memory_list;
	do {
		if(current->size >= size && !current->used)
		{
			uint32_t block_address = (uint32_t) current->start;
			uint32_t offset = ((block_address + alignment - 1) & -alignment) - block_address;

			if(offset == 0)
			{
				if(current->size > size)
					mm_split(current, size);

				current->used = true;
				total_free_memory -= current->size;
				retval = current->start;
				break;
			} else if(current->size - (offset + sizeof(struct memory_block)) >= size
				&& offset >= sizeof(struct memory_block) + MINIMUM_BLOCK_SIZE)
			{
				struct memory_block * split_block = mm_split(current, offset - sizeof(struct memory_block));
				if(split_block != 0)
				{
					if(split_block->size > size)
						mm_split(split_block, size);

					split_block->used = true;
					total_free_memory -= split_block->size;
					retval = split_block->start;
					break;
				}
			}
		}

		current = current->next;
	} while(current != 0);

	if(total_free_memory <= CONFIG_MIN_FREE_MEM)
	{
		// TODO: expand dataspace here.
	}

	return retval;
}

void mm_free(void * area)
{
	struct memory_block * block = (void *) ((uint32_t) area - sizeof(struct memory_block));
	block->used = false;
	total_free_memory += block->size;

	if(block->prev != 0 && !block->prev->used)
	{
		block->prev->size += block->size + sizeof(struct memory_block);
		total_free_memory += sizeof(struct memory_block);

		block->prev->next = block->next;
		if(block->next)
			block->next->prev = block->prev;
		block = block->prev;
	}

	if(block->next != 0 && !block->next->used)
	{
		block->size += block->next->size + sizeof(struct memory_block);
		total_free_memory += sizeof(struct memory_block);

		if(block->next->next)
			block->next->next->prev = block;
		block->next = block->next->next;
	}
}

static struct memory_block * mm_split(struct memory_block * block, unsigned offset)
{
	if(block->size <= offset + sizeof(struct memory_block) + MINIMUM_BLOCK_SIZE)
		return 0;

	size_t new_block_size = block->size - (offset + sizeof(struct memory_block));
	if(new_block_size >= MINIMUM_BLOCK_SIZE)
	{
		struct memory_block * retval = (struct memory_block *) ((uint32_t) block->start + offset);

		retval->size = new_block_size;
		retval->start = (void *) ((uint32_t) retval + sizeof(struct memory_block));
		retval->prev = block;
		retval->next = block->next;

		retval->used = false;

		if(block->next != 0)
			block->next->prev = retval;
		block->next = retval;
		block->size = offset;

		total_free_memory -= sizeof(struct memory_block);
		return retval;
	} else
		return 0;
}

void mm_add_physical(physical_ptr address, size_t size, unsigned int flags)
{
	struct memory_zone * new_zone = mm_allocate(sizeof(struct memory_zone), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);

	new_zone->start = address;
	new_zone->size = size & -CONFIG_PAGE_SIZE;
	new_zone->flags = flags;
	new_zone->next = physical_memory_list;

	new_zone->buddy_lists = mm_allocate(sizeof(uint8_t *) * CONFIG_BUDDY_MAX_ORDER, MM_DEFAULT_ALIGNMENT,
		MM_MEM_NORMAL);
	memclr(new_zone->buddy_lists, sizeof(uint8_t *) * CONFIG_BUDDY_MAX_ORDER);

	// Allocate buddy lists:
	for(unsigned i = 0; i <= CONFIG_BUDDY_MAX_ORDER; ++i)
	{
		new_zone->buddy_lists[i] = mm_allocate(sizeof(uint8_t *) * ((order_bits(i, size) + 7) / 8),
			MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
		memclr(new_zone->buddy_lists[i], sizeof(uint8_t *) * (((size & -CONFIG_PAGE_SIZE) >> (log2(CONFIG_PAGE_SIZE) + i)) >> 3));
	}

	// Set all maximum order blocks as unused:
	for(unsigned i = 0; i < (order_bits(CONFIG_BUDDY_MAX_ORDER, size) + 7) >> 3; ++i)
		new_zone->buddy_lists[CONFIG_BUDDY_MAX_ORDER][i] = 0xff;

	struct memory_zone * prev_zone = physical_memory_list;
	if(prev_zone == 0)
		physical_memory_list = new_zone;
	else {
		while(prev_zone->next != 0)
			prev_zone = prev_zone->next;
		prev_zone->next = new_zone;
	}
}

bool mm_allocate_physical(size_t size, struct mm_physical_memory * retval)
{
	struct memory_zone * zone = physical_memory_list; // FIXME: find a proper zone
	unsigned order = size_to_order(size);

	// Allocates a block of the specified order:
	retval->base = mm_allocate_order(zone, order);
	retval->size = order_blocksize(order);
	retval->flags = zone->flags;

	return true;
}

static physical_ptr mm_allocate_order(struct memory_zone * zone, unsigned order)
{
	for(unsigned i = 0; i < order_bits(order, zone->size); ++i)
	{
		if(zone->buddy_lists[order][i >> 3] & (1 << (i % 8)))
		{
			zone->buddy_lists[order][i >> 3] &= ~(1 << (i % 8));
			return (physical_ptr) ((uint32_t) zone->start + (i * order_blocksize(order)));
		}
	}

	if(order != CONFIG_BUDDY_MAX_ORDER)
	{
		physical_ptr split_block = mm_allocate_order(zone, order + 1);
		unsigned split_bits = ((uint32_t) split_block - (uint32_t) zone->start) >> log2(order_blocksize(order));
		// Return the first block, and set the other as unused:
		zone->buddy_lists[order][(split_bits + 1) >> 3] |= (1 << ((split_bits + 1) % 8));
		return split_block;
	} else // Cannot find a free block, return 0:
		return 0;
}

void mm_free_physical(struct mm_physical_memory * block)
{
	struct memory_zone * zone = physical_memory_list;
	while(zone != 0)
	{
		if((uint32_t) block->base >= (uint32_t) zone->start
			&& (uint32_t) block->base + block->size <= (uint32_t) zone->start + zone->size)
				break;
		zone = zone->next;
	}

	if(zone == 0)
		return;

	unsigned order = size_to_order(block->size);
	unsigned bitnum = ((uint32_t) block->base - (uint32_t) zone->start) >> (log2(CONFIG_PAGE_SIZE) + order);
	mm_free_order(zone, order, bitnum);
}

static void mm_free_order(struct memory_zone * zone, unsigned order, unsigned bitnum)
{
	zone->buddy_lists[order][bitnum >> 3] |= (1 << (bitnum % 8));

	if(order == CONFIG_BUDDY_MAX_ORDER)
		return;
	if(bitnum & 1)
	{
		if(zone->buddy_lists[order][(bitnum - 1) >> 3] & (1 << ((bitnum - 1) % 8)))
		{
			zone->buddy_lists[order][(bitnum - 1) >> 3] &= ~((3 << (bitnum - 1) % 8));
			mm_free_order(zone, order + 1, bitnum >> 1);
		}
	} else {
		if(zone->buddy_lists[order][(bitnum + 1) >> 3] & (1 << ((bitnum + 1) % 8)))
		{
			zone->buddy_lists[order][bitnum >> 3] &= ~((0x3 << bitnum % 8));
			mm_free_order(zone, order + 1, bitnum >> 1);
		}
	}
}

static void mm_reserve_block(struct memory_zone * zone, unsigned order, unsigned bitnum);
void mm_reserve_physical(physical_ptr address, size_t size)
{
	struct memory_zone * zone = physical_memory_list;
	while(zone != 0)
	{
		if((uint32_t) address >= (uint32_t) zone->start
			&& (uint32_t) address + size <= (uint32_t) zone->start + zone->size)
				break;
		zone = zone->next;
	}
	if(zone == 0)
		return;

	unsigned first_bitnum = ((uint32_t) address - (uint32_t) zone->start) >> log2(CONFIG_PAGE_SIZE);
	unsigned last_bitnum = (((uint32_t) address - (uint32_t) zone->start) + size) >> log2(CONFIG_PAGE_SIZE);
	for(unsigned i = first_bitnum; i < last_bitnum; ++i)
		mm_reserve_block(zone, 0, i);
}

static void mm_reserve_block(struct memory_zone * zone, unsigned order, unsigned bitnum)
{
	zone->buddy_lists[order][bitnum >> 3] &= ~(1 << (bitnum % 8));
	if(order == CONFIG_BUDDY_MAX_ORDER)
		return;

	mm_reserve_block(zone, order + 1, bitnum >> 1);
}

static inline unsigned size_to_order(size_t size)
{
	size_t fullsize = (size + CONFIG_PAGE_SIZE - 1) & -CONFIG_PAGE_SIZE;
	unsigned order;

	for(order = CONFIG_BUDDY_MAX_ORDER; order >= 0; --order)
		if((fullsize & -(1 << (log2(CONFIG_PAGE_SIZE) + order))) == fullsize)
			break;
	return order;
}

static inline unsigned order_bits(unsigned order, size_t size)
{
	return (size & -CONFIG_PAGE_SIZE) >> (log2(CONFIG_PAGE_SIZE) + order);
}

static inline size_t order_blocksize(unsigned order)
{
	return 1 << (log2(CONFIG_PAGE_SIZE) + order);
}

