// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_KERNEL_H
#define MORDAX_KERNEL_H

#include "dt.h"
#include "api/types.h"

extern struct dt * kernel_dt;

/**
 * Initializes the kernel and starts the initial process.
 * @param device_tree pointer to the physical address of the device tree.
 * @param dt_size size of the device tree.
 */
void kernel_main(physical_ptr * device_tree, size_t dt_size) __attribute((noreturn));

/**
 * Causes a kernel panic.
 * @param message the error message to print.
 */
void kernel_panic(const char * message) __attribute((weak,noreturn));

#endif

