@ The Mordax Microkernel
@ (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

.global idle_thread_loop
idle_thread_loop:
	wfi
	b idle_thread_loop

