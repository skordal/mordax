// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "context.h"
#include "registers.h"

#include "../context.h"
#include "../debug.h"
#include "../mm.h"
#include "../utils.h"

struct thread_context * context_new(void)
{
	struct thread_context * retval = mm_allocate(sizeof(struct thread_context),
		MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	memclr(retval, sizeof(struct thread_context));

	// Set the initial mode to user-mode, ARM instruction mode:
	retval->spsr = PROCESSOR_MODE_USR;
	return retval;
}

void context_free(struct thread_context * context)
{
	mm_free(context);
}

void context_copy(struct thread_context * dest, struct thread_context * src)
{
	memcpy(dest, src, sizeof(struct thread_context));
}

void * context_get_syscall_argument(struct thread_context * context, unsigned num)
{
	if(num > 3)
		return 0;
	else
		return (void *) context->r[num];
}

void context_set_syscall_retval(struct thread_context * context, void * value)
{
	context->r[0] = (uint32_t) value;
}

void context_print(struct thread_context * context)
{
	debug_printf("Register contents:\n");
	for(int i = 0; i < 15; i += 4)
	{
		if(i + 3 < 15)
			debug_printf("\tr%d: %x\tr%d: %x\tr%d: %x\tr%d: %x\n",
				i, context->r[i],
				i + 1, context->r[i + 1],
				i + 2, context->r[i + 2],
				i + 3, context->r[i + 3]);
		else
			debug_printf("\tr%d: %x\tr%d: %x\tr%d: %x\n",
				i, context->r[i],
				i + 1, context->r[i + 1],
				i + 2, context->r[i + 2]);
	}
	debug_printf("\tPC: %x\tSPSR: %x\n", context->pc, context->spsr);
}

void context_set_mode(struct thread_context * context, enum context_processor_mode mode)
{
	context->spsr &= ~(PROCESSOR_MODE_MASK);
	if(mode == CONTEXT_USERMODE)
		context->spsr |= PROCESSOR_MODE_USR;
	else
		context->spsr |= PROCESSOR_MODE_SYS;
}

void context_set_pc(struct thread_context * context, void * pc)
{
	context->pc = (uint32_t) pc;
}

void context_set_sp(struct thread_context * context, void * sp)
{
	context->r[13] = (uint32_t) sp;
}

