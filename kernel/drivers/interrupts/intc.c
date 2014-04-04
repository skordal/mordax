// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "intc.h"
#include "intc-omap3.h"

#include <debug.h>
#include <utils.h>

struct intc_driver_list_entry
{
	const char * compatible;
	struct intc_driver * driver;
};

struct intc_driver intc_omap3_driver =
{
	.initialize = intc_omap3_initialize,
	.handle_irq = intc_omap3_handle_irq,
	.register_handler = intc_omap3_register_handler,
	.unregister_handler = intc_omap3_unregister_handler,
	.get_handler = intc_omap3_get_handler,
};

static struct intc_driver_list_entry drivers[] =
{
	{ "ti,omap3-intc", &intc_omap3_driver },
	{ 0, 0}
};

struct intc_driver * intc_driver_instantiate(struct dt_node * device_node)
{
	const char * compatible = dt_get_string_property(device_node, "compatible");

	for(unsigned i = 0; drivers[i].compatible != 0; ++i)
	{
		if(str_equals(drivers[i].compatible, compatible))
		{
			debug_printf("Interrupt controller driver: %s\n", compatible);
			drivers[i].driver->initialize(device_node);
			return drivers[i].driver;
		}
	}

	return 0;
}

