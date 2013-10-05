// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "mm.h"
#include "stack.h"

struct stack_node
{
	void * data;
	struct stack_node * next;
};

struct stack
{
	struct stack_node * top;
};

struct stack * stack_new(void)
{
	struct stack * retval = mm_allocate(sizeof(struct stack), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	retval->top = 0;

	return retval;
}

void stack_free(struct stack * s, stack_free_func free_func)
{
	void * freed_element = 0;
	while(stack_pop(s, &freed_element))
		if(free_func)
			mm_free(freed_element);

	mm_free(s);
}

void stack_push(struct stack * s, void * e)
{
	struct stack_node * node = mm_allocate(sizeof(struct stack_node), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	node->data = e;
	node->next = s->top;
	s->top = node;
}

bool stack_pop(struct stack * s, void ** e)
{
	if(s->top == 0)
	{
		*e = 0;
		return false;
	} else {
		struct stack_node * node = s->top;
		s->top = node->next;
		*e = node->data;
		mm_free(node);
		return true;
	}
}

