# The Mordax System Call Library
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

.PHONY: all clean
ifeq ($(TOPLEVEL),)
        $(error "Please run make from the toplevel directory.")
endif

include config.mk
include $(ARCH)/config.mk

ASSEMBLER_FILES +=
OBJECT_FILES   := $(ASSEMBLER_FILES:.S=.o)

all: $(OBJECT_FILES)
	$(TARGET_AR) rcs libmordax.a $(OBJECT_FILES)

clean:
	-$(RM) $(ASSEMBLER_FILES:.S=.o)
	-$(RM) libmordax.a
	-$(RM) -r mordax

link-headers:
	test -d mordax || mkdir mordax
	for file in $$(ls -1 $(TOPLEVEL)/kernel/api); do \
		if [ ! -f mordax/$$file ]; then \
			$(LN) -sf $(TOPLEVEL)/kernel/api/$$file mordax/$$file; \
		fi; \
	done

install: all
	test -d $(TARGET_LIBDIR) || mkdir -p $(TARGET_LIBDIR)
	$(CP) -u libmordax.a $(TARGET_LIBDIR)
	test -d $(TARGET_INCLUDEDIR) || mkdir -p $(TARGET_INCLUDEDIR)
	$(CP) -u mordax.h mordax-ipc.h mordax-dt.h $(TARGET_INCLUDEDIR)
	test -d $(TARGET_INCLUDEDIR)/mordax || mkdir -p $(TARGET_INCLUDEDIR)/mordax
	for file in $$(ls -1 mordax/); do \
		if [ ! -f $(TARGET_INCLUDEDIR)/mordax/$$file ]; then \
			$(CP) mordax/$$file $(TARGET_INCLUDEDIR)/mordax/$$file; \
		fi; \
	done

$(OBJECT_FILES): link-headers

%.o: %.S
	$(TARGET_CC) -c $(TARGET_ASFLAGS) -o $@ $<

%.o: %.c %.h
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<

%.o: %.c
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<

