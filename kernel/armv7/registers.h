// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_ARMv7_REGISTERS_H
#define MORDAX_ARMv7_REGISTERS_H

/**
 * @defgroup arm_reg ARMv7 Processor Registers
 * @ingroup armv7
 * @{
 */

/**
 * @defgroup arm_modes ARM Processor Modes
 * @{
 */

#define PROCESSOR_MODE_USR	0b10000
#define PROCESSOR_MODE_FIQ	0b10001
#define PROCESSOR_MODE_IRQ	0b10010
#define PROCESSOR_MODE_SVC	0b10011
#define PROCESSOR_MODE_ABT	0b10111
#define PROCESSOR_MODE_UND	0b11011
#define PROCESSOR_MODE_SYS	0b11111
#define PROCESSOR_MODE_MASK	0b11111

/** @} */

/**
 * @defgroup nmrr Normal Memory Remap Register (NMRR) Defines
 * @{
 */

/** Non-cacheable region. */
#define NMRR_NC		0b00
/** Write-back, write-allocate region. */
#define NMRR_WB_WA	0b01
/** Write-through, no write-allocate region. */
#define NMRR_WT_NWA	0b10
/** Write-back, no write-allocate region. */
#define NMRR_WB_NWA	0b11

/** Bit number of the specified outer region. */
#define NMRR_OR(x)	(((x) << 1) + 16)
/** Bit number of the specified inner region. */
#define NMRR_IR(x)	((x) << 1)

/** @} */

/**
 * @defgroup prrr Primary Region Remap Register (PRRR) Defines
 * @{
 */

#define PRRR_NS1	19
#define PRRR_NS0	18
#define PRRR_DS1	17
#define PRRR_DS0	16

/** Strongly ordered memory region. */
#define PRRR_TR_SO	0b00
/** Device memory region. */
#define PRRR_TR_DEV	0b01
/** Normal memory region. */
#define PRRR_TR_NORM	0b10

/** Bit number of the specified memory region. */
#define PRRR_TR(x)	((x) << 1)

/** @} */

/**
 * @defgroup sctlr System Control Register (SCTLR) Defines
 * @{
 */

#define SCTLR_TRE	28
#define SCTLR_V		13
#define SCTLR_I		12
#define SCTLR_Z		11
#define SCTLR_C		 2
#define SCTLR_M		 0

/** @} */

/**
 * @defgroup acr Auxilliary Control Register (ACR) Defines
 * @{
 */

#define ACR_L1NEON	5
#define ACR_ASA		4

/** @} */
/** @} */

#endif

