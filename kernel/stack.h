// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_STACK_H
#define MORDAX_STACK_H

#include "api/types.h"

/**
 * @defgroup stack Stack functions
 * @{
 */

/** Abstract stack type. */
struct stack;

/** Function type for freeing stack elements. */
typedef void (*stack_free_func)(void *);

/**
 * Allocates a new, empty stack.
 */
struct stack * stack_new(void);

/**
 * Frees a stack and, optionally, all elements on the stack.
 * @param s the stack to free.
 * @param free_func a function to use for freeing elements on the stack. If
 *                  this is set to `null`, the elements are not freed.
 */
void stack_free(struct stack * s, stack_free_func free_func);

/**
 * Pushes an element onto a stack.
 * @param s the stack.
 * @param e the element to push onto the stack.
 */
void stack_push(struct stack * s, void * e);

/**
 * Pops an element off a stack.
 * @param s the stack.
 * @param e a pointer to store the popped value.
 * @return `true` if an element was successfully popped off the stack, `false`
 *         if there were no elements to pop off the stack.
 */
bool stack_pop(struct stack * s, void ** e);

/** @} */

#endif

