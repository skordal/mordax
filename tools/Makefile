# The Mordax Microkernel OS Tools
# (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# This file (and the makefiles in subdirectories) needs the toplevel
# configuration files:
ifeq ($(TOPLEVEL),)
        $(error "Please run make from the toplevel directory.")
endif

TOOLS ?= \
	mkinitproc

.PHONY: all clean $(TOOLS)

all: $(TOOLS)

clean:
	@for dir in $(TOOLS); do \
		$(MAKE) -C $$dir clean; \
	done

$(TOOLS):
	@$(MAKE) -C $@

