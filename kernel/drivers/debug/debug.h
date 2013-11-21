// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_DRIVERS_DEBUG_H
#define MORDAX_DRIVERS_DEBUG_H

#include <dt.h>
#include <api/types.h>

/** Function used for initializing a debug terminal. */
typedef bool (*debug_driver_init_func)(struct dt_node * device_node);

/** Function for printing a character to a debug device. */
typedef void (*debug_driver_putc_func)(char c);
/**
 * Function for printing a string to a debug device. This function is optional,
 * and provided to allow drivers to provide an optimized way to print strings.
 */
typedef void (*debug_driver_puts_func)(const char * s, size_t length);

struct debug_driver
{
	debug_driver_init_func initialize;
	debug_driver_putc_func putc;
};

/**
 * Instantiates a debug driver based on device tree node.
 * @param device_node the device tree node describing the device to use for the debug terminal.
 * @return a debug_driver structure to make use of the driver.
 */
struct debug_driver * debug_driver_instantiate(struct dt_node * device_node);

#endif

