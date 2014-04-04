// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 - 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_DRIVERS_TIMER_TIMER_OMAP3_H
#define MORDAX_DRIVERS_TIMER_TIMER_OMAP3_H

#include <dt.h>
#include <types.h>

#define TIMER_OMAP3_FCLK	12000000

// Register offsets:
#define TIMER_OMAP3_TIOCP_CFG	(0x10 >> 2)
#define TIMER_OMAP3_TISTAT	(0x14 >> 2)
#define TIMER_OMAP3_TISR	(0x18 >> 2)
#define TIMER_OMAP3_TIER	(0x1c >> 2)
#define TIMER_OMAP3_TCLR	(0x24 >> 2)
#define TIMER_OMAP3_TLDR	(0x2c >> 2)
#define TIMER_OMAP3_TTGR	(0x30 >> 2)

// TIOCP_CFG bitnames:
#define TIMER_OMAP3_IDLEMODE	3
#define TIMER_OMAP3_SOFTRESET	1
#define TIMER_OMAP3_AUTOIDLE	0

// TISTAT bitnames:
#define TIMER_OMAP3_RESETDONE	0

// TISR bitnames:
#define TIMER_OMAP3_OVF_IT_FLAG	1

// TIER bitnames:
#define TIMER_OMAP3_OVF_IT_ENA	1

// TCLR bitnames:
#define TIMER_OMAP3_TRG		10
#define TIMER_OMAP3_AR		 1
#define TIMER_OMAP3_ST		 0

bool timer_omap3_initialize(struct dt_node * device_node);
void timer_omap3_set_interval(unsigned int interval);
void timer_omap3_set_callback(timer_callback_func callback);
void timer_omap3_start(void);
void timer_omap3_stop(void);

#endif

