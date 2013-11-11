# The Mordax Microkernel OS System Call Test Programme
# (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

.section .text

simpletest_start:
	mov r0, #0		@ Choose syscall 0, function 0
	ldr r1, =test_message	@ Load the address of the message
	svc #0			@ Call syscall 0, system

1:
	svc #1			@ Call syscall 1, thread exit
	wfi			@ This is (hopefully) never reached
	b 1b

.section .data
test_message:
	.asciz "Simple Mordax test application\n"

