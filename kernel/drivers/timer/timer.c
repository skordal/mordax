// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <debug.h>
#include <utils.h>

#include "timer.h"
#include "timer-omap3.h"

struct timer_driver_list_entry
{
	const char * compatible;
	struct timer_driver * driver;
};

struct timer_driver timer_omap3_driver =
{
	.initialize = timer_omap3_initialize,
	.set_interval = timer_omap3_set_interval,
	.set_callback = timer_omap3_set_callback,
	.start = timer_omap3_start,
	.stop = timer_omap3_stop
};

static struct timer_driver_list_entry drivers[] =
{
	{ "ti,omap3-timer", &timer_omap3_driver },
	{ 0, 0}
};

struct timer_driver * timer_driver_instantiate(struct dt_node * device_node)
{
	const char * compatible = dt_get_string_property(device_node, "compatible");

	for(unsigned i = 0; drivers[i].compatible != 0; ++i)
	{
		if(str_equals(drivers[i].compatible, compatible))
		{
			debug_printf("Timer driver: %s\n", compatible);
			drivers[i].driver->initialize(device_node);
			return drivers[i].driver;
		}
	}

	return 0;
}



