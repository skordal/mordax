# The Mordax Microkernel OS
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

.PHONY: all clean kernel tools

export TARGET
export TOPLEVEL = $(PWD)

include configs/host-config.mk

ifeq ($(TARGET),)
        $(error "No target specified!")
else
        # Extract the architecture from the target name:
        export ARCH     := $(firstword $(subst -, ,$(TARGET)))
        export PLATFORM := $(word 2,$(subst -, ,$(TARGET)))
        include configs/$(ARCH)-config.mk
endif

all: kernel

kernel:
	@$(MAKE) -C kernel

clean:
	@$(MAKE) -C kernel clean

