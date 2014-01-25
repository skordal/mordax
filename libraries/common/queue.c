// The Mordax Operating System Common Modules Library
// (c) Kristian Klomsten Skordal <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "queue.h"

#ifdef COMPILING_KERNEL
#	include "mm.h"
#	include "utils.h"
#	define malloc(size)	mm_allocate(size, MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL)
#	define free(x)		mm_free(x)
#else
#	include <stdlib.h>
#	include <string.h>
#endif

struct queue * queue_new(void)
{
	struct queue * retval = malloc(sizeof(struct queue));
	memset(retval, 0, sizeof(struct queue));
	return retval;
}

void queue_free(struct queue * q, queue_data_free_func free_func)
{
	struct queue_node * current = q->first;
	while(current != 0)
	{
		struct queue_node * temp = current;
		if(free_func)
			free_func(temp->data);
		current = current->next;
		free(temp);
	}

	free(q);
}

void queue_add_front(struct queue * q, void * e)
{
	struct queue_node * new_node = malloc(sizeof(struct queue_node));
	new_node->data = e;
	new_node->next = q->first;
	new_node->prev = 0;

	if(q->first != 0)
		q->first->prev = new_node;
	else
		q->last = new_node;

	q->first = new_node;
	++q->elements;
}

void queue_add_back(struct queue * q, void * e)
{
	struct queue_node * new_node = malloc(sizeof(struct queue_node));
	new_node->data = e;
	new_node->next = 0;
	new_node->prev = q->last;

	if(q->last != 0)
		q->last->next = new_node;
	else
		q->first = new_node;

	q->last = new_node;
	++q->elements;
}

bool queue_remove_front(struct queue * q, void ** e)
{
	if(q->first == 0)
		return false;
	else {
		struct queue_node * node = q->first;

		q->first = node->next;
		if(q->first != 0)
			q->first->prev = 0;
		else
			q->last = 0;
		--q->elements;

		*e = node->data;
		free(node);
		return true;
	}
}

bool queue_remove_back(struct queue * q, void ** e)
{
	if(q->last == 0)
		return false;
	else {
		struct queue_node * node = q->last;

		q->last = node->prev;
		if(q->last != 0)
			q->last->next = 0;
		else
			q->first = 0;
		--q->elements;

		*e = node->data;
		free(node);
		return true;
	}
}

void * queue_remove_node(struct queue * q, struct queue_node * node)
{
	if(node == 0)
		return 0;

	void * retval = node->data;

	if(node->prev)
		node->prev->next = node->next;
	if(node->next)
		node->next->prev = node->prev;

	if(q->first == node)
		q->first = node->next;
	if(q->last == node)
		q->last = node->prev;

	free(node);
	return retval;

}

