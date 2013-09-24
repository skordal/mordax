The Mordax Microkernel OS
=========================

This project aims to create a simple operating system with its own kernel and applications.

How to build
------------

The following tools are currently required to build Mordax:
* An `arm-eabi-none` toolchain.
* The device tree compiler, `dtc`.
* `mkimage` from the U-Boot project.
* Various Unix utilities which you probably have anyway. Try to build the project and you will find out what you are missing.

When you have the required tools, run `make` from the toplevel folder. You have to specify a target using the `TARGET` makefile variable on the command line. The only working target at the moment is `TARGET=armv7-beagleboard`. Another target that will be supported is `TARGET=armv7-am335x_evm`, but it may not work yet.

The Microkernel
---------------

The microkernel is a work in progress and is changing rapidly.

### What works

* Initial boot code (though the initial stack allocation should probably be improved).
* Physical memory management using a simple buddy allocator.
* Virtual memory management for the kernel using a simple linked-list first-fit allocator (with O(n) for allocation = slow for many allocations).
* Device tree parsing and usage.

### What does not work/What sort-of works
* The red-black tree implementation probably does not work. Problems that pops up have been fixed by inserting more null checks into the insertion fixup function, so it is likely that the trees are unbalanced and wrong. Also, deletion is not supported, but required for the kernel's use of the trees. A new implementation is being written.
* Adding more blocks of memory to the physical memory manager.
* Multiple platform support. The output of the `debug_printf` function is hardcoded to the UART on the Beagleboard-Xm at the moment. A board-specific `early_printf` function is being written. Using an application accessible (through IPC) buffer for storing debug messages is also planned.
* The 16c750 UART driver is incomplete.
* The interrupt controller driver is incomplete.
* Interrupt support in general is mostly unimplemented.
* Abort handling is incomplete.

### What will work

* Timer support
* A scheduler
* IPC
* Threads and processes
* Probably more exciting features!

