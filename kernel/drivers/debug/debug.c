// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "uart-16c750.h"

#include <debug.h>
#include <utils.h>

struct debug_driver_list_entry
{
	const char * compatible; // TODO: implement support for multiple compatible strings per driver
	struct debug_driver * driver;
};

static struct debug_driver uart_16c750_driver =
{
	.initialize = uart_16c750_initialize,
	.putc = uart_16c750_putc,
};

static struct debug_driver_list_entry drivers[] =
{
	{ "national,16c750", &uart_16c750_driver },
	{ 0, 0 }
};

struct debug_driver * debug_driver_instantiate(struct dt_node * device_node)
{
	const char * compatible = dt_get_string_property(device_node, "compatible");

	for(unsigned i = 0; drivers[i].compatible != 0; ++i)
	{
		if(str_equals(drivers[i].compatible, compatible))
		{
			debug_printf("Debug driver: %s\n", compatible);
			drivers[i].driver->initialize(device_node);
			return drivers[i].driver;
		}
	}

	return 0;
}

