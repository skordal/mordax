# The Mordax Microkernel OS
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>

# Target independent build configuration

# Build tools:
export HOST_CC ?= $(CC)

# Build flags:
export HOST_CFLAGS  ?= $(CFLAGS) -O2 -Wall -g -std=gnu11
export HOST_LDFLAGS ?= $(LDFLAGS)

# Target independent build tools:
export CP    ?= cp
export FIND  ?= find
export LN    ?= ln
export MKDIR ?= mkdir
export TEST  ?= test

