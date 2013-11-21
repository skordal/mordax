// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_KERNEL_H
#define MORDAX_KERNEL_H

#include "api/types.h"

/**
 * Initializes the kernel and starts the initial process.
 * @param device_tree pointer to the physical address of the device tree.
 * @param dt_size size of the device tree.
 */
void kernel_main(physical_ptr * device_tree, size_t dt_size);

/**
 * Causes a kernel panic.
 * @param message the error message to print.
 */
void kernel_panic(const char * message) __attribute((weak,noreturn));

#endif

