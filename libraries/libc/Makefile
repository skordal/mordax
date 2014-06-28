# The Mordax Standard Library
# (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

.PHONY: all clean link-common clean-common
ifeq ($(TOPLEVEL),)
        $(error "Please run make from the toplevel directory.")
endif

include config.mk
include $(ARCH)/config.mk

ASSEMBLER_FILES +=
SOURCE_FILES += \
	errno.c \
	initialize.c \
	memcpy.c \
	memset.c \
	mm.c \
	printf.c \
	strcmp.c
OBJECT_FILES := $(ASSEMBLER_FILES:.S=.o) $(SOURCE_FILES:.c=.o)

TARGET_CFLAGS  += -isystem $(TOPLEVEL)/libraries/libc/include -I$(TARGET_INCLUDEDIR)
TARGET_ASFLAGS += -I$(TARGET_INCLUDEDIR)

# Modules imported from common:
COMMON_MODULES := rbtree
COMMON_OBJECTS := $(foreach module,$(COMMON_MODULES),$(module).o)

all: link-common libc.a

libc.a: $(COMMON_OBJECTS) $(OBJECT_FILES)
	$(TARGET_AR) rcs libc.a $@ $^

clean: clean-common
	-$(RM) $(OBJECT_FILES) $(COMMON_OBJECTS) libc.a

install:
	$(TEST) -d $(TARGET_LIBDIR) || $(MKDIR) -p $(TARGET_LIBDIR)
	$(CP) -u libc.a $(TARGET_LIBDIR)
	$(TEST) -d $(TARGET_INCLUDEDIR) || $(MKDIR) -p $(TARGET_INCLUDEDIR)
	$(CP) -u include/*.h $(TARGET_INCLUDEDIR)
	$(TEST) -d $(TARGET_INCLUDEDIR)/sys || $(MKDIR) -p $(TARGET_INCLUDEDIR)/sys
	$(CP) -u include/sys/* $(TARGET_INCLUDEDIR)/sys

link-common:
	for module in $(COMMON_MODULES); do \
		$(TEST) -L include/$${module}.h || $(LN) -s ../../common/$${module}.h include/$${module}.h; \
		$(TEST) -L $${module}.c || $(LN) -s ../common/$${module}.c $${module}.c; \
	done

clean-common:
	-$(RM) $(foreach temp,$(COMMON_MODULES),include/$(temp).h)
	-$(RM) $(foreach temp,$(COMMON_MODULES),$(temp).c)

$(COMMON_OBJECTS): link-common
	$(TARGET_CC) -c $(TARGET_CFLAGS) -Iinclude -o $@ $(@:.o=.c)

%.o: %.S
	$(TARGET_CC) -c $(TARGET_ASFLAGS) -o $@ $<

%.o: %.c %.h
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<

%.o: %.c
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<

