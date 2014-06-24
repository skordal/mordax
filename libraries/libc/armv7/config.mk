# The Mordax Standard Library
# (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# ARMv7 build flags:
TARGET_CFLAGS  += -mfpu=neon -mcpu=cortex-a8
TARGET_ASFLAGS += -mfpu=neon -mcpu=cortex-a8

# ARMv7 specific source files:
ASSEMBLER_FILES += \
	armv7/crt0.S \
	armv7/uidiv.S
SOURCE_FILES    +=

