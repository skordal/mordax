# The Mordax System Call Library
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# Target independent build flags:
TARGET_ASFLAGS += -Wall -O2 -ffreestanding -std=gnu11 \
	-I$(TOPLEVEL)/libraries/mordax
TARGET_CFLAGS  += -Wall -O2 -ffreestanding -std=gnu11 \
	-Werror=implicit-function-declaration \
	-I$(TOPLEVEL)/libraries/mordax
TARGET_LDFLAGS += -nostdlib

