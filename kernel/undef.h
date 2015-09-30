// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_UNDEF_H
#define MORDAX_UNDEF_H

/**
 * Undefined instruction handler.
 * @param context context of the thread that caused the exception.
 */
void undef_interrupt_handler(struct thread_context * context);

#endif

