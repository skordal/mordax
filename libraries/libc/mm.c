// The Mordax Standard Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mordax.h>
#include "mm.h"

// Minimum size of a memory block:
#define MINIMUM_BLOCKSIZE	4

struct memory_block
{
	size_t size;
	bool used;
	struct memory_block * next, * prev;
};

extern void * __application_end;
static void * program_break;
static unsigned int free_memory;

// Set the first memory block to be at the beginning of free memory:
static struct memory_block * first_block = (struct memory_block *) &__application_end;

// Splits a memory block:
static void split_block(struct memory_block * block, unsigned offset);

void __mm_initialize(void)
{
	program_break = (void *) (((uint32_t) &__application_end + 4095) & -4096);
	free_memory = (uint32_t) program_break - (uint32_t) __application_end;

	// FIXME: The following code assumes there is enough memory for the first block.
	memset(first_block, 0, sizeof(struct memory_block));
	first_block->size = free_memory - sizeof(struct memory_block);
	first_block->used = false;

	// Initially allocate an extra page of memory for use when initializing
	// and to make sure there is enough memory to initialize:
	sbrk(4096);
}

void * sbrk(size_t incr)
{
	// Round up to a multiple of the page size:
	incr = (incr + 4095) & -4096;

	struct mordax_memory_attributes attributes = {
		.type = MORDAX_TYPE_CODE,
		.permissions = MORDAX_PERM_RW_RW
	};

	// Increase the dataspace in page-sized intervals:
	for(int i = 0; i < incr / 4096; ++i)
	{
		void * target = program_break;
		size_t size = 4096;
		mordax_memory_map_alloc(program_break, &size, &attributes);

		program_break = (void *) ((uint32_t) target + size);
		free_memory += size;
	}

	struct memory_block * last_block = first_block;
	while(last_block->next != NULL)
		last_block = last_block->next;

	if(last_block->used)
	{
		struct memory_block * new_block = (void *) ((uint32_t) last_block + sizeof(struct memory_block) + last_block->size);
		new_block->used = false;
		new_block->size = incr - sizeof(struct memory_block);

		last_block->next = new_block;
		new_block->prev = last_block;
	} else
		last_block->size += incr;

	return program_break;
}

void * malloc(size_t size)
{
	void * retval = NULL;
	bool second_try = false;
	struct memory_block * current;

	// Make size a multiple of 4:
	size = (size + 3) & -4;
	
	if(size > free_memory)
		sbrk(size + sizeof(struct memory_block));

_malloc_retry:
	current = first_block;
	while(current != NULL)
	{
		if(!current->used && current->size >= size)
		{
			retval = (void *) ((uint32_t) current + sizeof(struct memory_block));
			current->used = true;
			if(current->size > size)
				split_block(current, size);
			break;
		}

		current = current->next;
		if(current == NULL && !second_try)
		{
			sbrk(size + sizeof(struct memory_block));
			second_try = true;
			goto _malloc_retry;
		}
	}

	if(retval == NULL)
		errno = ENOMEM;
	return retval;
}

void free(void * ptr)
{
	struct memory_block * block = (void *) ((uint32_t) ptr - sizeof(struct memory_block));
	block->used = false;

	if(block->next != NULL && !block->next->used)
	{
		block->size += block->next->size + sizeof(struct memory_block);
		block->next = block->next->next;
		if(block->next != NULL)
			block->next->prev = block;
	}

	if(block->prev != NULL && !block->prev->used)
	{
		if(block->next != NULL)
			block->next->prev = block->prev;

		block->prev->size += block->size + sizeof(struct memory_block);
		block->prev->next = block->next;
		block = block->prev;
	}
}

static void split_block(struct memory_block * block, unsigned offset)
{

	size_t new_block_size = block->size - offset - sizeof(struct memory_block);
	if(new_block_size < MINIMUM_BLOCKSIZE)
		return;

	// NOTE: The offset parameter refers the the offset into the allocateable memory area of a block.
	struct memory_block * new_block = (void *) ((uint32_t) block + sizeof(struct memory_block) + offset);
	new_block->size = new_block_size;
	new_block->used = false;
	new_block->prev = block;
	new_block->next = block->next;
	block->next = new_block;
}

