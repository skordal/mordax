// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_DRIVERS_DEBUG_UART_16C750_H
#define MORDAX_DRIVERS_DEBUG_UART_16C750_H

#include <dt.h>
#include <drivers/debug/debug.h>

#define DEBUG_16C750_DEFAULT_BAUDRATE	115200
#define DEBUG_16C750_FCLK		48000000

// Register definitions:
#define DEBUG_16C750_DLL	0x00
#define DEBUG_16C750_DLH	0x04
#define DEBUG_16C750_IER	0x04
#define DEBUG_16C750_FCR	0x08
#define DEBUG_16C750_EFR	0x08
#define DEBUG_16C750_LCR	0x0c
#define DEBUG_16C750_MDR1	0x20
#define DEBUG_16C750_SYSC	0x54
#define DEBUG_16C750_SYSS	0x58

// IER bit names:
#define DEBUG_16C750_SLEEP_MODE		4

// EFR bit names:
#define DEBUG_16C750_ENHANCED_EN	4

// FCR bit names:
#define DEBUG_16C750_FIFO_EN		0

// LCR bit names:
#define DEBUG_16C750_DIV_EN		7
#define DEBUG_16C750_PARITY_TYPE1	4
#define DEBUG_16C750_PARITY_EN		3
#define DEBUG_16C750_CHAR_LENGTH	0

// MDR1 bit names:
#define DEBUG_16C750_MODE_SELECT	0

// SYSC bit names:
#define DEBUG_16C750_IDLEMODE		3
#define DEBUG_16C750_ENAWAKEUP		2
#define DEBUG_16C750_SOFTRESET		1
#define DEBUG_16C750_AUTOIDLE		0

// SYSS bit names:
#define DEBUG_16C750_RESETDONE		0

bool uart_16c750_initialize(struct dt_node * device_node);
void uart_16c750_putc(char c);

#endif

