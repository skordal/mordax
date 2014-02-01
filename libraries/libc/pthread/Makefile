# The Mordax POSIX Thread Emulation Library
# (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

.PHONY: all clean
ifeq ($(TOPLEVEL),)
        $(error "Please run make from the toplevel directory.")
endif

include config.mk
include $(ARCH)/config.mk

SOURCE_FILES += pthread.c
OBJECT_FILES := $(ASSEMBLER_FILES:.S=.o) $(SOURCE_FILES:.c=.o)

TARGET_CFLAGS += -I.. -isystem $(TARGET_INCLUDEDIR)

all: $(OBJECT_FILES)
	$(TARGET_AR) rcs libpthread.a $(OBJECT_FILES)

clean:
	-$(RM) $(OBJECT_FILES) libpthread.a

install:
	#$(TEST) -d $(TARGET_LIBDIR) || $(MKDIR) -p $(TARGET_LIBDIR)
	#$(CP) -u libpthread.a $(TARGET_LIBDIR)
	$(TEST) -d $(TARGET_INCLUDEDIR) || $(MKDIR) -p $(TARGET_INCLUDEDIR)
	$(CP) -u pthread.h $(TARGET_INCLUDEDIR)

%.o: %.S
	$(TARGET_CC) -c $(TARGET_ASFLAGS) -o $@ $<

%.o: %.c %.h
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<

%.o: %.c
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<

