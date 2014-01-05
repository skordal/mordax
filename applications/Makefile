# The Mordax Microkernel OS
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# This file (and the makefiles in subdirectories) needs the toplevel
# configuration files:
ifeq ($(TOPLEVEL),)
        $(error "Please run make from the toplevel directory.")
endif

# Application flags:
TARGET_CFLAGS += -I$(TOPLEVEL)/libraries/mordax -O2 -Wall -std=gnu11
TARGET_LDFLAGS += -L$(TOPLEVEL)/libraries/mordax

# List of applications to build:
APPLICATIONS ?= \
	syscall_tests
.PHONY: all clean $(APPLICATIONS)

all: $(APPLICATIONS)

clean:
	@for dir in $(APPLICATIONS); do \
		$(MAKE) -C $$dir clean; \
	done

simple-crt0.o: simple-crt0.S
	$(TARGET_CC) -c $(TARGET_CFLAGS) -o simple-crt0.o simple-crt0.S

$(APPLICATIONS): simple-crt0.o
	@$(MAKE) -C $@

