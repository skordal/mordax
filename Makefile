# The Mordax Microkernel OS
# (c) Kristian Klomsten Skordal 2013 - 2014 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>
.PHONY: all applications clean kernel libraries tools

export TARGET
export TOPLEVEL = $(CURDIR)

include configs/host-config.mk

ifeq ($(TARGET),)
        $(error "No target specified!")
else
        # Extract the architecture from the target name:
        export ARCH     := $(firstword $(subst -, ,$(TARGET)))
        export PLATFORM := $(word 2,$(subst -, ,$(TARGET)))
        include configs/$(ARCH)-config.mk
endif

#Set the initial application for the kernel build process:
export KERNEL_INITPROC ?= initproc.bin
# Set the applications to add to the initial process archive:
INITIAL_APPLICATIONS   ?= applications/services/vfs/vfs.elf

# Set the destination for compiled libraries and programmes:
export TARGET_INSTALL_PREFIX ?= $(TOPLEVEL)/target
export TARGET_INCLUDEDIR     ?= $(TARGET_INSTALL_PREFIX)/usr/include
export TARGET_LIBDIR         ?= $(TARGET_INSTALL_PREFIX)/lib

all: prepare-destdir kernel applications tools make-initproc

prepare-destdir:
	$(MKDIR) -p $(TARGET_INSTALL_PREFIX)

make-initproc: tools applications
	tools/mkinitproc/mkinitproc tools/mkinitproc/$(ARCH)-loader.bin \
		$(INITIAL_APPLICATIONS) > /dev/null

applications: prepare-destdir libraries tools
	@$(MAKE) -C applications

kernel: applications make-initproc
	@$(MAKE) -C kernel
	$(CP) kernel/mordax.itb .

libraries: prepare-destdir
	@$(MAKE) -C libraries

tools: libraries
	@$(MAKE) -C tools

clean:
	-$(RM) -r $(TARGET_INSTALL_PREFIX) initproc.bin mordax.itb
	@$(MAKE) -C applications clean
	@$(MAKE) -C kernel clean
	@$(MAKE) -C libraries clean
	@$(MAKE) -C tools clean
