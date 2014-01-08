// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_NUMBER_ALLOCATOR_H
#define MORDAX_NUMBER_ALLOCATOR_H

/**
 * @defgroup numalloc Number Allocator
 * @{
 */

struct number_allocator;

/**
 * Creates a new number allocator.
 * @return a pointer to a new number allocator.
 */
struct number_allocator * number_allocator_new(void);

/**
 * Frees a number allocator.
 */
void number_allocator_free(struct number_allocator * alloc);

/**
 * Allocates a number from an allocator.
 * @param alloc the number allocator to allocate from.
 * @return the next possible number or 0 if a number could not be allocated.
 */
unsigned int number_allocator_allocate_num(struct number_allocator * alloc);

/**
 * Frees a previously allocated number.
 * @param alloc the number allocator to free to.
 * @param num the number to free.
 */
void number_allocator_free_num(struct number_allocator * alloc, unsigned int num);

/** @} */

#endif

