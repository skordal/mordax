The Mordax Operating System
===========================

This project aims to create a simple operating system with its own kernel and applications.

How to build
------------

The following tools are currently required to build Mordax:
* An `arm-eabi-none` toolchain.
* The device tree compiler, `dtc`.
* `mkimage` from the U-Boot project.
* Various Unix utilities which you probably have anyway (unless you are running Windows). Try to build, and you will find out what you are missing.

When you have the required tools, run `make` from the toplevel folder. You have to specify a target using the `TARGET` makefile variable on the command line. The supported targets are `armv7-beagleboard` for the Beagleboard-xM and `armv7-am335x_evm` for Texas Instruments' AM335x Evaluation Board.

Beware, that until there is a stable release, there may be issues in the code causing the build to fail.

The kernel
----------

The kernel is a mostly-microkernel, which aims to provide the least amount of neccessary functionality for applications to run. It is a work in progress, and currently only supports a limited set of system calls while being stricly single-processor (no locking or synchronization of any kind has been included yet).

The Applications
----------------

* mt_test: a simple application used for testing the multithreading capabilities of the kernel.

The Libraries
-------------

* libmordax: a library providing wrapper functions for the supported system calls.

