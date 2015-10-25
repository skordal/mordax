# The Mordax Microkernel
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

.PHONY: all clean link-common
ifeq ($(TOPLEVEL),)
        $(error "Please run make from the toplevel directory.")
endif

include config.mk
include $(ARCH)/config.mk

TARGET_ASFLAGS += $(CONFIG)
TARGET_CFLAGS  += $(CONFIG)
OBJECT_FILES   := $(ASSEMBLER_FILES:.S=.o) $(SOURCE_FILES:.c=.o)

# Modules to import from the common library:
COMMON_MODULES := queue rbtree stack
COMMON_OBJECTS := $(foreach module,$(COMMON_MODULES),$(module).o)

all: $(COMMON_OBJECTS) $(OBJECT_FILES)
	$(TARGET_LD) -T $(TARGET_LDSCRIPT) -Wl,-Map,mordax.map -o mordax.elf \
		$(OBJECT_FILES) $(COMMON_OBJECTS) $(TARGET_LDFLAGS)
	$(TARGET_SIZE) mordax.elf > mordax.size
	$(CP) $(TOPLEVEL)/$(KERNEL_INITPROC) init.bin
	$(TARGET_OBJCOPY) -O binary -j .init -j .text -j .data \
		mordax.elf mordax.bin
	$(GZIP) -qf9 mordax.bin
	$(DTC) -O dtb -o mordax.dtb $(DTS_FILE)
	$(MKIMAGE) -f mordax.its mordax.itb > /dev/null
	$(CAT) mordax.size

clean: clean-common
	-$(RM) $(OBJECT_FILES)
	-$(RM) $(COMMON_OBJECTS)
	-$(RM) mordax.elf mordax.bin mordax.bin.gz mordax.dtb mordax.itb

link-common:
	for module in $(COMMON_MODULES); do \
		$(TEST) -L $${module}.h || $(LN) -s ../libraries/common/$${module}.h $${module}.h; \
		$(TEST) -L $${module}.c || $(LN) -s ../libraries/common/$${module}.c $${module}.c; \
	done

clean-common:
	-$(RM) $(foreach temp,$(COMMON_MODULES),$(temp).h)
	-$(RM) $(foreach temp,$(COMMON_MODULES),$(temp).c)

$(COMMON_OBJECTS): link-common
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $(@:.o=.c)

%.o: %.S
	$(TARGET_CC) -c $(TARGET_ASFLAGS) -o $@ $<

%.o: %.c %.h api/types.h
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<

%.o: %.c api/types.h
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<

