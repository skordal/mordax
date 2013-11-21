// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_LOCK_H
#define MORDAX_LOCK_H

#include "api/types.h"

/** Lock type. */
typedef uint32_t lock_t;

/**
 * Initializes a lock to an unlocked state.
 * @return an unlocked lock.
 */
lock_t lock_initialize(void);

/**
 * Locks a lock. Spins until the lock can be locked.
 * @param lock the lock to lock.
 */
void lock_lock(lock_t * lock) __attribute((weak));

/**
 * Unlocks a lock.
 * @param lock the lock to unlock.
 */
void lock_unlock(lock_t * lock) __attribute((weak));

/**
 * Tests if a lock is locked.
 * @param lock the lock to test.
 */
bool lock_is_locked(lock_t * lock) __attribute((weak));

#endif

