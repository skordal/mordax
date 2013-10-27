// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_ARMv7_CONTEXT_H
#define MORDAX_ARMv7_CONTEXT_H

#include "../context.h"
#include "../types.h"

/** Flag value indicating a need to store Neon register on context switches. */
#define CONTEXT_STORE_NEON_STATE	(1 << 0)

/**
 * @ingroup armv7
 * @{
 */

/** Stored thread context structure. */
struct thread_context
{
	uint32_t spsr;	//< Stored status register
	uint32_t pc;	//< Exception return address
	uint32_t r[15];	//< Registers r0 - r14
	uint32_t flags;	//< Flags, such as whether to store Neon context.
} __attribute((packed));

/** @} */

#endif

