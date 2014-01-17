# The Mordax Microkernel OS
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>
.PHONY: all clean

TESTAPPS ?= ipc_test mt_test map_test

all: $(TESTAPPS)

$(TESTAPPS):
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o $@.o $@.c
	$(TARGET_LD) $(TARGET_LDFLAGS) -T ../simple.ld ../simple-crt0.o $@.o -lmordax -o $@.elf
	$(TARGET_OBJCOPY) -O binary -j .text -j .data $@.elf $@.bin

clean:
	-$(RM) $(foreach temp,$(TESTAPPS),$(temp).o)
	-$(RM) $(foreach temp,$(TESTAPPS),$(temp).elf)
	-$(RM) $(foreach temp,$(TESTAPPS),$(temp).bin)

