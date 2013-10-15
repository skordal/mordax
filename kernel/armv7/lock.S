@ The Mordax Microkernel
@ (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
@ Report bugs and issues on <http://github.com/skordal/mordax/issues>
.syntax unified
.arm

#define LOCK_LOCKED_VALUE	0xffffffff
#define LOCK_UNLOCKED_VALUE	0x00000000

.section .text

@ Initializes a lock with the unlocked value.
@ Returns:
@	r0 - unlocked lock value
.global lock_initialize
.type lock_initialize, %function
lock_initialize:
	mov r0, #LOCK_UNLOCKED_VALUE
	mov pc, lr

@ Locks a lock. Spins until the lock can be locked.
@ Parameters:
@	r0 - pointer to the lock
.global lock_lock
.type lock_lock, %function
lock_lock:
1:
	ldrex r1, [r0]
	cmp r1, #LOCK_LOCKED_VALUE
	itt eq
	wfeeq
	beq 1b

2:
	mov r1, #LOCK_LOCKED_VALUE
	strex r2, r1, [r0]
	cmp r2, #0
	it ne
	bne 2b

	dmb
	mov pc, lr

@ Unlocks a lock.
@ Parameters:
@	r0 - pointer to the lock
.global lock_unlock
.type lock_unlock, %function
lock_unlock:
	dmb

	mov r1, #LOCK_UNLOCKED_VALUE
	str r1, [r0]
	dsb
	sev

	mov pc, lr

@ Tests if a lock is locked.
@ Parameters:
@	r0 - pointer to the lock
@ Returns:
@	r0 - 1 if the lock is locked, 0 otherwise
.global lock_is_locked
.type lock_is_locked, %function
lock_is_locked:
	ldr r1, [r0]
	cmp r1, #LOCK_LOCKED_VALUE
	ite eq
	moveq r0, #1
	movne r0, #0
	mov pc, lr

