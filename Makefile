# The Mordax Microkernel OS
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>
.PHONY: all applications clean kernel libraries

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

# Set the initial application for the kernel build process:
export KERNEL_INITPROC ?= applications/syscall_tests/mt_test.bin

all: kernel applications

applications: libraries
	@$(MAKE) -C applications

kernel: applications
	@$(MAKE) -C kernel

libraries:
	@$(MAKE) -C libraries

clean:
	@$(MAKE) -C applications clean
	@$(MAKE) -C kernel clean
	@$(MAKE) -C libraries clean
