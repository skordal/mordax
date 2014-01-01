# The Mordax Microkernel OS
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# ARMv7 build configuration.

# Target prefix:
TARGET_PREFIX  ?= arm-none-eabi

# Target-dependent build tools:
export TARGET_CC      ?= $(TARGET_PREFIX)-gcc
export TARGET_LD      ?= $(TARGET_PREFIX)-ld
export TARGET_AR      ?= $(TARGET_PREFIX)-ar
export TARGET_OBJCOPY ?= $(TARGET_PREFIX)-objcopy

# Target-dependent build flags:
export TARGET_ASFLAGS  += -mno-unaligned-access
export TARGET_CFLAGS   += -mno-unaligned-access
export TARGET_LDFLAGS  += 

