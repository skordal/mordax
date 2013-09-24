// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_ARMv7_CONTEXT_H
#define MORDAX_ARMv7_CONTEXT_H

#include "../types.h"

/**
 * @ingroup armv7
 * @{
 */

/** Stored thread context structure. */
struct thread_context
{
	uint32_t r[15];	//< Registers r0 - r14
	uint32_t pc;	//< Exception return address
	uint32_t spsr;	//< Stored status register
} __attribute((packed));

/** @} */

#endif

