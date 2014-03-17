# The Mordax Microkernel
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# Include platform files:
include armv7/platforms/$(PLATFORM).mk

# ARMv7 kernel configuration:
CONFIG += \
	-DCONFIG_PAGE_SIZE=4096 \
	-DCONFIG_BUDDY_MAX_ORDER=8 \
	-DCONFIG_INITIAL_HEAP_SIZE=8192 \
	-DCONFIG_MIN_FREE_MEM=4096 \
	-DCONFIG_SYSTEM_STACK_SIZE=32768 \
	-DCONFIG_DEFAULT_STACK_SIZE=32768 \
	-DCONFIG_DEFAULT_STACK_BASE=\(0x80000000U-CONFIG_DEFAULT_STACK_SIZE\) \
	-DCONFIG_LITTLE_ENDIAN \
	-DCONFIG_KERNEL_SPLIT=0x80000000U \
	-DCONFIG_IPC_BUFFER_LENGTH=4096

# Target linker script:
TARGET_LDSCRIPT := armv7/mordax.ld

# Target dependent source files:
ASSEMBLER_FILES += \
	armv7/bootstrap.S \
	armv7/endian.S \
	armv7/idle_thread.S \
	armv7/interrupts.S \
	armv7/log2.S \
	armv7/memcpy.S \
	armv7/uidiv.S
SOURCE_FILES += \
	armv7/abort.c \
	armv7/context.c \
	armv7/mmu.c \
	armv7/stack_setup.c

