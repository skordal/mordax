// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_ARMv7_MMU_H
#define MORDAX_ARMv7_MMU_H

#include "registers.h"

/**
 * @defgroup arm_mmu ARMv7 MMU Functions and Definitions
 * @ingroup armv7
 * @{
 */

#define MMU_KERNEL_SPLIT_ADDRESS	0x80000000U

/**
 * @defgroup arm_mmu_pt Page Table Defines
 * @{
 */

#define MMU_PAGE_TABLE_TYPE		0b01
#define MMU_PAGE_TABLE_BASE_MASK	0xfffffc00

/** @} */

/**
 * @defgroup arm_mmu_sp Small Page Defines
 * @{
 */

#define MMU_SMALL_PAGE_TYPE		0b10
#define MMU_SMALL_PAGE_BASE_MASK	0xfffff000

#define MMU_SMALL_PAGE_XN		 0
#define MMU_SMALL_PAGE_B		 2
#define MMU_SMALL_PAGE_C		 3
#define MMU_SMALL_PAGE_AP0		 4
#define MMU_SMALL_PAGE_AP1		 5
#define MMU_SMALL_PAGE_TEX		 6
#define MMU_SMALL_PAGE_AP2		 9
#define MMU_SMALL_PAGE_S		10
#define MMU_SMALL_PAGE_NG		11

#define MMU_SMALL_PAGE_CODE		0
#define MMU_SMALL_PAGE_RODATA		(1 << MMU_SMALL_PAGE_B)
#define MMU_SMALL_PAGE_DATA		(1 << MMU_SMALL_PAGE_C)
#define MMU_SMALL_PAGE_STACK		(1 << MMU_SMALL_PAGE_C | 1 << MMU_SMALL_PAGE_B)
#define MMU_SMALL_PAGE_STRORD		(1 << MMU_SMALL_PAGE_TEX)
#define MMU_SMALL_PAGE_DEVICE		MMU_SMALL_PAGE_UNCACHED
#define MMU_SMALL_PAGE_UNCACHED		(1 << MMU_SMALL_PAGE_TEX | 1 << MMU_SMALL_PAGE_C | 1 << MMU_SMALL_PAGE_B)

#define MMU_SMALL_PAGE_NA_NA		0
#define MMU_SMALL_PAGE_RW_NA		(1 << MMU_SMALL_PAGE_AP0)
#define MMU_SMALL_PAGE_RW_RO		(1 << MMU_SMALL_PAGE_AP1)
#define MMU_SMALL_PAGE_RW_RW		(1 << MMU_SMALL_PAGE_AP0 | 1 << MMU_SMALL_PAGE_AP1)
#define MMU_SMALL_PAGE_RO_NA		(1 << MMU_SMALL_PAGE_AP2 | 1 << MMU_SMALL_PAGE_AP0)
#define MMU_SMALL_PAGE_RO_RO		(1 << MMU_SMALL_PAGE_AP2 | 1 << MMU_SMALL_PAGE_AP1 | 1 << MMU_SMALL_PAGE_AP0)

/** @} */

/**
 * @defgroup arm_mmu_section Section Entry Defines
 * @{
 */

#define MMU_SECTION_TYPE	0b10

#define MMU_SECTION_B		 2
#define MMU_SECTION_C		 3
#define MMU_SECTION_XN		 4
#define MMU_SECTION_DOMAIN	 5
#define MMU_SECTION_AP0		10
#define MMU_SECTION_AP1		11
#define MMU_SECTION_TEX		12
#define MMU_SECTION_AP2		15
#define MMU_SECTION_S		16
#define MMU_SECTION_NG		17
#define MMU_SECTION_NS		19

#define MMU_SECTION_BASE	20
#define MMU_SECTION_BASE_MASK	0xfff00000

#define MMU_SECTION_CODE	0
#define MMU_SECTION_RODATA	(1 << MMU_SECTION_B)
#define MMU_SECTOIN_DATA	(1 << MMU_SECTION_C)
#define MMU_SECTION_STACK	(1 << MMU_SECTION_C | 1 << MMU_SECTION_B)
#define MMU_SECTION_STRORD	(1 << MMU_SECTION_TEX)
// FIXME: using the device type does not work on the Beagleboard Xm:
// #define MMU_SECTION_DEVICE	(1 << MMU_SECTION_TEX | 1 << MMU_SECTION_B)
#define MMU_SECTION_DEVICE	MMU_SECTION_UNCACHED
#define MMU_SECTION_UNCACHED	(1 << MMU_SECTION_TEX | 1 << MMU_SECTION_C | 1 << MMU_SECTION_B)

#define MMU_SECTION_NA_NA	0
#define MMU_SECTION_RW_NA	(1 << MMU_SECTION_AP0)
#define MMU_SECTION_RW_RO	(1 << MMU_SECTION_AP1)
#define MMU_SECTION_RW_RW	(1 << MMU_SECTION_AP0 | 1 << MMU_SECTION_AP1)
#define MMU_SECTION_RO_NA	(1 << MMU_SECTION_AP2 | 1 << MMU_SECTION_AP0)
#define MMU_SECTION_RO_RO	(1 << MMU_SECTION_AP2 | 1 << MMU_SECTION_AP1 | 1 << MMU_SECTION_AP0)

/** @} */
/** @} */

#endif

