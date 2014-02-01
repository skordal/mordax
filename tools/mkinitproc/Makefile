# The Mordax Microkernel OS Tools
# (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>
.PHONY: all clean $(ARCH)-loader.bin

# This file (and the makefiles in subdirectories) needs the toplevel
# configuration files:
ifeq ($(TOPLEVEL),)
        $(error "Please run make from the toplevel directory.")
endif

SOURCE_FILES := \
	mkinitproc.c
OBJECT_FILES := $(SOURCE_FILES:.c=.o)

HOST_CFLAGS  +=
HOST_LDFLAGS += -lelf

TARGET_CFLAGS  += -I$(TARGET_INCLUDEDIR)
TARGET_LDFLAGS += -L$(TARGET_LIBDIR) -lmordax

all: $(ARCH)-loader.bin $(OBJECT_FILES)
	$(HOST_CC) $(HOST_CFLAGS) -o mkinitproc $(HOST_LDFLAGS) $(OBJECT_FILES)

clean:
	-$(RM) $(OBJECT_FILES) mkinitproc
	-$(RM) $(ARCH)-loader.o $(ARCH)-loader.elf $(ARCH)-loader.bin

$(ARCH)-loader.o: $(ARCH)-loader.c
	$(TARGET_CC) -c -o $@ $(TARGET_CFLAGS) $<
$(ARCH)-loader.bin: $(ARCH)-loader.o $(ARCH)-loader.ld
	$(TARGET_LD) -T $(ARCH)-loader.ld -o $(ARCH)-loader.elf $(TARGET_LDFLAGS) $(ARCH)-loader.o -lmordax
	$(TARGET_OBJCOPY) -O binary -j .text $(ARCH)-loader.elf $(ARCH)-loader.bin

%.o: %.c
	$(HOST_CC) -c $(HOST_CFLAGS) -o $@ $<

%.o: %.c %.h
	$(HOST_CC) -c $(HOST_CFLAGS) -o $@ $<

