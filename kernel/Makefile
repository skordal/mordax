# The Mordax Microkernel
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

.PHONY: all clean
ifeq ($(TOPLEVEL),)
        $(error "Please run make from the toplevel directory.")
endif

include config.mk
include $(ARCH)/config.mk

TARGET_ASFLAGS += $(CONFIG)
TARGET_CFLAGS  += $(CONFIG)
OBJECT_FILES   := $(ASSEMBLER_FILES:.S=.o) $(SOURCE_FILES:.c=.o)

all: $(OBJECT_FILES)
	$(TARGET_LD) $(TARGET_LDFLAGS) -T $(TARGET_LDSCRIPT) -Map mordax.map -o mordax.elf \
		$(OBJECT_FILES)
	$(CP) $(TOPLEVEL)/$(KERNEL_INITPROC) init.bin
	$(TARGET_OBJCOPY) -O binary -j .init -j .text -j .data \
		mordax.elf mordax.bin
	$(GZIP) -qf9 mordax.bin
	$(DTC) -O dtb -o mordax.dtb $(DTS_FILE)
	$(MKIMAGE) -f mordax.its mordax.itb

clean:
	-$(RM) $(ASSEMBLER_FILES:.S=.o)
	-$(RM) $(SOURCE_FILES:.c=.o)
	-$(RM) mordax.elf mordax.bin mordax.bin.gz mordax.dtb mordax.itb

%.o: %.S
	$(TARGET_CC) -c $(TARGET_ASFLAGS) -o $@ $<

%.o: %.c %.h api/types.h
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<

%.o: %.c api/types.h
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<

