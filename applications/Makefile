# The Mordax Microkernel OS
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# This file (and the makefiles in subdirectories) needs the toplevel
# configuration files:
ifeq ($(TOPLEVEL),)
        $(error "Please run make from the toplevel directory.")
endif

# List of applications to build:
APPLICATIONS ?=
.PHONY: all clean $(APPLICATIONS)

all: $(APPLICATIONS)

clean:
	@for dir in $(APPLICATIONS); do \
		$(MAKE) -C $$dir clean; \
	done

$(APPLICATIONS):
	@$(MAKE) -C $@

