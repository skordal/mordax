# The Mordax Microkernel OS
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# ARMv7 build configuration.

# Target prefix:
TARGET_PREFIX  ?= arm-none-eabi
TARGET_CFLAGS  += -mcpu=cortex-a8

# Target-dependent build tools:
export TARGET_CC      ?= $(TARGET_PREFIX)-gcc
export TARGET_LD      ?= $(TARGET_PREFIX)-gcc
export TARGET_AR      ?= $(TARGET_PREFIX)-ar
export TARGET_OBJCOPY ?= $(TARGET_PREFIX)-objcopy
export TARGET_SIZE    ?= $(TARGET_PREFIX)-size

# Target-dependent build flags:
export TARGET_ASFLAGS  += -mno-unaligned-access -Wall -O2
export TARGET_CFLAGS   += -mno-unaligned-access -std=gnu11 -Wall -O2 -fomit-frame-pointer
export TARGET_LDFLAGS  +=  -nostartfiles

