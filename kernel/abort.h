// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_ABORT_H
#define MORDAX_ABORT_H

#include "api/types.h"

struct thread_context;

/** Structure providing details about an abort. */
struct abort_details
{
	enum { ABORT_READ, ABORT_WRITE } type;
	enum { ABORT_KERNEL, ABORT_USER } mode;
	void * address;
};

/**
 * Abort handler function. An abort occurs when an application tries to access
 * memory that it does not have permissions to access, or memory that is not
 * mapped.
 * @param context context of the faulting thread.
 * @return `true` if the faulting instruction should be retried, `false` otherwise.
 */
bool abort_handler(struct thread_context * context);

/**
 * Target specific function providing details about the abort.
 * @param details pointer to a structure that the function should fill in.
 */
void abort_get_details(struct abort_details * details, struct thread_context * context);

#endif

