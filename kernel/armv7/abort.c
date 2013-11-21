// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "../abort.h"
#include "../debug.h"
#include "../api/types.h"

#include "context.h"
#include "registers.h"

void abort_get_details(struct abort_details * details, struct thread_context * context)
{
	// Read the DFSR register:
	uint32_t dfsr;
	asm volatile("mrc p15, 0, %[dfsr], c5, c0, 0\n\t" : [dfsr] "=r" (dfsr));

	// Read the IFSR register:
	uint32_t ifsr;
	asm volatile("mrc p15, 0, %[ifsr], c5, c0, 1\n\t" : [ifsr] "=r" (ifsr));

	if(dfsr == 0)
	{
		// Read the IFAR register to get the faulting address:
		asm volatile("mrc p15, 0, %[address], c6, c0, 2\n\t" : [address] "=r" (details->address));
		details->type = ABORT_READ;
	} else {
		// Read the DFAR register to get the faulting address:
		asm volatile("mrc p15, 0, %[address], c6, c0, 0\n\t" : [address] "=r" (details->address));
		// Check if the access was a read or a write:
		details->type = dfsr & (1 << 11) ? ABORT_WRITE : ABORT_READ;
	}

	uint8_t mode = context->spsr & 0x1f;
	if(mode == PROCESSOR_MODE_SYS || mode == PROCESSOR_MODE_SVC)
		details->mode = ABORT_KERNEL;
	else
		details->mode = ABORT_USER;
}

