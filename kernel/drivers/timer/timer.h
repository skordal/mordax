// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_DRIVERS_TIMER_TIMER_H
#define MORDAX_DRIVERS_TIMER_TIMER_H

#include <dt.h>
#include <types.h>

/** Timer callback function. */
typedef void (*timer_callback_func)(void);

/** Function type for timer initialization functions. */
typedef bool (*timer_driver_init_func)(struct dt_node * device_node);
/** Function type for setting the timer interval in microseconds. */
typedef void (*timer_driver_set_interval_func)(unsigned int interval);
/** Function type for setting the timer callback function. */
typedef void (*timer_driver_set_callback_func)(timer_callback_func callback);
/** Function type for starting and stopping the timer. */
typedef void (*timer_driver_startstop_func)(void);

/** Timer driver structure. */
struct timer_driver
{
	timer_driver_init_func initialize;
	timer_driver_set_interval_func set_interval;
	timer_driver_set_callback_func set_callback;
	timer_driver_startstop_func start, stop;
};

struct timer_driver * timer_driver_instantiate(struct dt_node * device_node);

#endif

