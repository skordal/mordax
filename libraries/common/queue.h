// The Mordax Operating System Common Modules Library
// (c) Kristian Klomsten Skordal <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_QUEUE_H
#define MORDAX_QUEUE_H

#include <stdbool.h>

/**
 * @defgroup queue Queue support
 * Functionality for working with queues.
 * @{
 */

/** Function for freeing data in the queue. */
typedef void (*queue_data_free_func)(void *);

struct queue_node
{
	void * data;
	struct queue_node * next, * prev;
	// (next is the element to the right of the node, prev is to the left when visualising)
};

struct queue
{
	struct queue_node * first, * last;
	unsigned int elements;
};

/**
 * Allocates a new, empty queue.
 * @return a newly allocated, empty queue.
 */
struct queue * queue_new(void) __attribute((malloc));

/**
 * Frees a queue.
 * @param queue the queue to free.
 */
void queue_free(struct queue * q, queue_data_free_func free_func);

/**
 * Adds an element to the front of a queue.
 * @param q the queue to add to.
 * @param e the element to add to the queue.
 */
void queue_add_front(struct queue * q, void * e);

/**
 * Adds an element to the back of a queue.
 * @param q the queue to add to.
 * @param e the element to remove from the queue.
 */
void queue_add_back(struct queue * q, void * e);

/**
 * Removes an element to the front of a queue.
 * @param q the queue to remove from.
 * @param e pointer to where the removed element should be stored.
 * @return `true` if an element was removed, `false` otherwise.
 */
bool queue_remove_front(struct queue * q, void ** e);

/**
 * Removes an element from the back of a queue.
 * @param q the queue to remove from.
 * @param e pointer to where the removed element should be stored.
 * @return `true` if an element was removed, `false` otherwise.
 */
bool queue_remove_back(struct queue * q, void ** e);

/**
 * Removes a node from a queue. The node is freed in the process.
 * @param q the queue to remove from.
 * @param node the node to remove from the queue.
 * @return the data that was stored in the node.
 */
void * queue_remove_node(struct queue * q, struct queue_node * node);


/** @} */

#endif

