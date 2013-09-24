// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <debug.h>
#include <mmu.h>

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
	debug_printf("Debug UART memory area is at %x physical, %x virtual, length %d\n", memory_info[0], memory, memory_info[1]);

	return true;
}

void uart_16c750_putc(char c)
{
	while(!(memory[0x14] & (1 << 5)));
	*memory = c;
}

