# The Mordax Microkernel
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# Device tree file for the AM335x Evaluation Module:
DTS_FILE ?= armv7/platforms/am335x_evm.dts

# Processor flags:
TARGET_CFLAGS  += -mcpu=cortex-a8
TARGET_ASFLAGS += -mcpu=cortex-a8

