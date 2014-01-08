// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "mm.h"
#include "number_allocator.h"
#include "rbtree.h"

struct number_allocator
{
	unsigned int next_num;
	struct rbtree * allocated_nums;
};

struct number_allocator * number_allocator_new(void)
{
	struct number_allocator * retval = mm_allocate(sizeof(struct number_allocator *),
		MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	retval->allocated_nums = rbtree_new(0, 0, 0, 0);
	retval->next_num = 0;
	return retval;
}

void number_allocator_free(struct number_allocator * alloc)
{
	rbtree_free(alloc->allocated_nums);
	mm_free(alloc);
}

unsigned int number_allocator_allocate_num(struct number_allocator * alloc)
{
	unsigned int retval = ++alloc->next_num;
	unsigned int first_attempt = retval;

	while(rbtree_key_exists(alloc->allocated_nums, (void *) retval))
	{
		retval = ++alloc->next_num;
		if(retval == 0)
			retval = ++alloc->next_num;

		// Check if all possibilities have been tried:
		if(first_attempt == retval)
			return 0;
	}

	rbtree_insert(alloc->allocated_nums, (void *) retval, (void *) 1);
	return retval;
}

void number_allocator_free_num(struct number_allocator * alloc, unsigned int num)
{
	rbtree_delete(alloc->allocated_nums, (void *) num);
}

