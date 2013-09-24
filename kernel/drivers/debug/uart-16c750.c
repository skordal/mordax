// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <debug.h>
#include <mmu.h>
#include <utils.h>

#include <drivers/debug/uart-16c750.h>
#include <drivers/debug/debug.h>

static volatile char * memory;

bool uart_16c750_initialize(struct dt_node * device_node)
{
	uint32_t memory_info[2];
	if(!dt_get_array32_property(device_node, "reg", memory_info, 2))
	{
		debug_printf("Error: debug uart node \"%s\" has no \"reg\" property\n",
			device_node->name);
		return false;
	}

	memory = mmu_map_device((physical_ptr) memory_info[0], (size_t) memory_info[1]);

	// Reset the controller:
	memory[DEBUG_16C750_SYSC] |= 1 << DEBUG_16C750_SOFTRESET;
	while((memory[DEBUG_16C750_SYSS] & (1 << DEBUG_16C750_RESETDONE)) == 0);

	// Set up power management settings:
	memory[DEBUG_16C750_SYSC] =
		1 << DEBUG_16C750_AUTOIDLE |
		1 << DEBUG_16C750_ENAWAKEUP |
		2 << DEBUG_16C750_IDLEMODE; // Smart-idle mode

	// Enable sleep mode:
	const char * sleep_mode = dt_get_string_property(device_node, "mordax,uart-sleepmode");
	if(sleep_mode == 0 || str_equals(sleep_mode, "enabled"))
	{
		memory[DEBUG_16C750_EFR] = 1 << DEBUG_16C750_ENHANCED_EN;
		memory[DEBUG_16C750_IER] = 1 << DEBUG_16C750_SLEEP_MODE;
		memory[DEBUG_16C750_EFR] = 0;
	}

	// Switch to configuration mode A:
	memory[DEBUG_16C750_LCR] = 1 << DEBUG_16C750_DIV_EN;

	// Enable the FIFOs:
	memory[DEBUG_16C750_FCR] = 1 << DEBUG_16C750_FIFO_EN;

	// Set the baudrate:
	uint32_t baudrate = DEBUG_16C750_DEFAULT_BAUDRATE;
	dt_get_array32_property(device_node, "mordax,uart-speed", &baudrate, 1);

	uint16_t divisor = ((DEBUG_16C750_FCLK >> 4) / baudrate) & 0xffff;
	memory[DEBUG_16C750_DLL] = divisor & 0xff;
	memory[DEBUG_16C750_DLH] = divisor >> 8;

	// Set the transmission parameters (including parity mode) - returns to operational mode:
	const char * parity = dt_get_string_property(device_node, "mordax,uart-parity");
	if(parity == 0 || str_equals(parity, "none"))
		memory[DEBUG_16C750_LCR] = 3 << DEBUG_16C750_CHAR_LENGTH;
	else if(str_equals(parity, "even"))
		memory[DEBUG_16C750_LCR] = 3 << DEBUG_16C750_CHAR_LENGTH |
			1 << DEBUG_16C750_PARITY_EN |
			1 << DEBUG_16C750_PARITY_TYPE1;
	else if(str_equals(parity, "odd"))
		memory[DEBUG_16C750_LCR] = 3 << DEBUG_16C750_CHAR_LENGTH |
			1 << DEBUG_16C750_PARITY_EN;

	memory[DEBUG_16C750_MDR1] = 0; // Enable UART 16x mode
	return true;
}

void uart_16c750_putc(char c)
{
	while(!(memory[0x14] & (1 << 5)));
	*memory = c;
}

