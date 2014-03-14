// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_API_DT_H
#define MORDAX_API_DT_H

#include "types.h"

/**
 * Structure for passing strings to and returning from device tree system calls.
 */
struct mordax_dt_string
{
	char * string;
	size_t length;
};

#endif

