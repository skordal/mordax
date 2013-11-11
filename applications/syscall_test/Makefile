# The Mordax Microkernel OS
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

.PHONY: all clean

all: test.o test.ld
	$(TARGET_LD) $(TARGET_LDFLAGS) -T test.ld test.o -o test.elf
	$(TARGET_OBJCOPY) -O binary -j .text -j .data \
		test.elf test.bin

test.o: test.S
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o test.o test.S

clean:
	-$(RM) test.o test.elf test.bin

