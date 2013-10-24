The Mordax Microkernel OS
=========================

This project aims to create a simple operating system with its own kernel and applications.

How to build
------------

The following tools are currently required to build Mordax:
* An `arm-eabi-none` toolchain.
* The device tree compiler, `dtc`.
* `mkimage` from the U-Boot project.
* Various Unix utilities which you probably have anyway (unless you are running Windows). Try to build, and you will find out what you are missing.

When you have the required tools, run `make` from the toplevel folder. You have to specify a target using the `TARGET` makefile variable on the command line. The supported targets are `armv7-beagleboard` for the Beagleboard-xM and `armv7-am335x_evm` for Texas Instruments' AM335x Evaluation Board.

The Microkernel
---------------

The microkernel is a work in progress and is changing rapidly.

### What works

* Initial boot code (though the initial stack allocation should probably be improved).
* Physical memory management using a simple buddy allocator.
* Virtual memory management for the kernel using a simple linked-list first-fit allocator (with O(n) for allocation = quite slow).
* Device tree parsing and usage.
* Debug logging using an UART.
* IRQs.
* Timers.
* Stacks (the data structure).
* Red-black trees
* Queues
* Spinlocks.

### What does not work/What sort-of works
* Adding more blocks of memory to the physical memory manager.
* Extending the kernel heap when neccessary.
* Interrupt support is incomplete.
* Abort handling is incomplete.

### What will work

* A scheduler
* IPC
* Threads and processes
* Locking and thread-safeness
* Probably more exciting features!

