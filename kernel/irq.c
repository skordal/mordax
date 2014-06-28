// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 - 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "context.h"
#include "debug.h"
#include "irq.h"
#include "mm.h"
#include "scheduler.h"
#include "thread.h"

#include "api/errno.h"
#include "drivers/interrupts/intc.h"

static struct intc_driver * driver = 0;

// IRQ handler function, called from the assembly interrupt handler:
void irq_interrupt_handler(struct thread_context * context)
{
	driver->handle_irq(context);
}

// IRQ handler registered by IRQ objects:
static void irq_object_handler(struct thread_context * context, unsigned irq, void * data_ptr)
{
	struct irq_object * object = data_ptr;
	irq_disable(irq);

	if(object->listener)
	{
		context_set_syscall_retval(context, 0);
		scheduler_move_thread_to_running(object->listener);
		object->listener = 0;
	}
}

void irq_set_intc_driver(struct intc_driver * d)
{
	driver = d;
}

void irq_register(unsigned irq, irq_handler_func handler, void * data_ptr)
{
	driver->register_handler(irq, handler, data_ptr);
}

void irq_unregister(unsigned irq)
{
	driver->unregister_handler(irq);
}

void irq_enable(unsigned irq)
{
	driver->enable_irq(irq);
}

void irq_disable(unsigned irq)
{
	driver->disable_irq(irq);
}

irq_handler_func irq_get_handler(unsigned irq)
{
	return driver->get_handler(irq);
}

struct irq_object * irq_object_create(unsigned irq)
{
	if(irq_get_handler(irq) != 0)
		return 0;

	struct irq_object * retval = mm_allocate(sizeof(struct irq_object), MM_DEFAULT_ALIGNMENT,
		MM_MEM_NORMAL);
	retval->irq = irq;
	retval->listener = 0;

	irq_register(irq, irq_object_handler, retval);
	return retval;
}

void irq_object_destroy(struct irq_object * object)
{
	if(object->listener)
	{
		context_set_syscall_retval(object->listener->context, (void *) -EIDRM);
		scheduler_move_thread_to_running(object->listener);
	}

	irq_disable(object->irq);
	irq_unregister(object->irq);

	mm_free(object);
}

int irq_object_listen(struct irq_object * object, struct thread * listener, bool * blocking)
{
	if(object->listener)
	{
		*blocking = false;
		return -EBUSY;
	} else {
		object->listener = listener;
		*blocking = true;
		irq_enable(object->irq);
		return 0;
	}
}

