// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_DEBUG_H
#define MORDAX_DEBUG_H

#include "drivers/debug/debug.h"

/**
 * Sets the debug driver to use for outputting debug messages to a terminal.
 * @param driver the driver to use.
 */
void debug_set_output_driver(struct debug_driver * driver);

/**
 * Prints a formatted string to the debug terminal.
 * @param format string specifying the format of the output string.
 */
void debug_printf(const char * format, ...);

#endif

