// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_DRIVERS_DEBUG_UART_16C750_H
#define MORDAX_DRIVERS_DEBUG_UART_16C750_H

#include <dt.h>
#include <drivers/debug/debug.h>

bool uart_16c750_initialize(struct dt_node * device_node);
void uart_16c750_putc(char c);

#endif

