# The Mordax Microkernel
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# Target independent build tools:
DTC     ?= dtc
GZIP    ?= gzip
MAKE    ?= make
MKIMAGE ?= mkimage
RM      ?= rm

# Build flags:
TARGET_ASFLAGS += -Wall -g -O2 -ffreestanding -std=gnu11 -I$(TOPLEVEL)/kernel -DASSEMBLER
TARGET_CFLAGS  += -Wall -g -O2 -ffreestanding -std=gnu11 -I$(TOPLEVEL)/kernel
TARGET_LDFLAGS += -nostdlib

# Target independent source files:
SOURCE_FILES += \
	abort.c \
	debug.c \
	dt.c \
	irq.c \
	kernel.c \
	mm.c \
	rbtree.c \
	stack.c \
	utils.c

# Include build configuration files for the drivers:
include drivers/config.mk

