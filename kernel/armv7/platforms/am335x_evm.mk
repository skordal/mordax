# The Mordax Microkernel
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# Device tree file for the AM335x Evaluation Module:
DTS_FILE ?= armv7/platforms/am335x_evm.dts

# Processor flags:
TARGET_CFLAGS  += -mcpu=cortex-a8
TARGET_ASFLAGS += -mcpu=cortex-a8

# Extra configuration for early debugging support:
CONFIG += -DCONFIG_EARLY_DEBUG -DCONFIG_EARLY_DEBUG_ADDRESS=0x44e09000

# Early debugging support:
ASSEMBLER_FILES += \
	armv7/platforms/16c750-early.S

