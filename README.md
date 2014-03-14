The Mordax Operating System
===========================

This project aims to create a simple operating system with its own kernel and applications.

How to build
------------

The following tools are currently required to build Mordax:
* An `arm-eabi-none` toolchain.
* The device tree compiler, `dtc`.
* `mkimage` from the U-Boot project.
* `libelf` and a host compiler toolchain for building the `mkinitproc` tool.
* Various Unix utilities which you probably have anyway (unless you are running Windows). Try to build and you will find out what you are missing.

When you have the required tools, run `make` from the toplevel folder. You have to specify a target using the `TARGET` makefile variable on the command line. The supported targets are `armv7-beagleboard` for the Beagleboard-xM and `armv7-am335x_evm` for Texas Instruments' AM335x Evaluation Board. At the moment, the AM335x version does not boot correctly.

Beware, that until there is a stable release in some distant future, there may be issues in the code causing the build to fail.

The kernel
----------

The kernel is a microkernel to the greatest degree possible, but includes drivers for printing debug output to a serial console, memory management, device trees, and timer support for multitasking. It is a work in progress, and is currently stricly single-processor (no locking or synchronization is included, and the scheduler is primitive).

The Applications
----------------

* dt_test: a simple application used for testing the kernel device tree interface.
* ipc_test: a simple application used for testing IPC services and sockets.
* lock_test: a simple application used for testing the lock interface.
* map_test: a simple application used for testing memory mapping. It currently works only on the Beagleboard due to it mapping the UART.
* mt_test: a simple application used for testing the multithreading capabilities of the kernel.

The Libraries
-------------

* libmordax: a library providing wrapper functions for all supported system calls.
* libc: a standard C library implementation for Mordax applications.

