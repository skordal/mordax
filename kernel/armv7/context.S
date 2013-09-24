@ The Mordax Microkernel
@ (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

@ Stores the current task context:
.global context_store
.type context_store, %function
context_store:
	mov pc, lr

@ Restores the current task context from the register block in
@ the current_context variable:
.global context_restore
.type context_restore, %function
context_restore:
	mov pc, lr

.section .bss
@ Pointer to the register block to store the current context to or restore
@ the context from:
current_context:
	.4byte

