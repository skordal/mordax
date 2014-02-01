# The Mordax OS Libraries
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# List of libraries to build:
LIBRARIES ?= \
	mordax \
	libc

.PHONY: all clean $(LIBRARIES)

# This file (and the makefiles in subdirectories) needs the toplevel
# configuration files:
ifeq ($(TOPLEVEL),)
        $(error "Please run make from the toplevel directory.")
endif

all: $(LIBRARIES)

clean:
	@for dir in $(LIBRARIES); do \
		$(MAKE) -C $$dir clean; \
	done

$(LIBRARIES):
	@$(MAKE) -C $@ all install

# Library dependencies:
libc: mordax

