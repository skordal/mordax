# The Mordax Standard Library
# (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# Target independent build flags:
TARGET_ASFLAGS += -Wall -O2 -ffreestanding -std=gnu11
TARGET_CFLAGS  += -Wall -O2 -ffreestanding -std=gnu11 \
	-Werror=implicit-function-declaration
TARGET_LDFLAGS += -nostdlib -L$(TARGET_LIBDIR)


