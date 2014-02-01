// The Mordax Standard Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <string.h>
#include <mordax.h>

#include "mm.h"

// Initializes the POSIX threads implementation:
void __pthreads_initialize(void);

// Clears the .bss section of the executable:
static void clear_bss(void)
{
	extern void * __bss_start, * __application_end; // Symbols from the linker script.
	size_t bss_size = (unsigned int) &__application_end - (unsigned int) &__bss_start;
	memset(&__bss_start, 0, bss_size);
}

// Initializes the standard library:
void __initialize(void)
{
	clear_bss();
	__mm_initialize();
}



