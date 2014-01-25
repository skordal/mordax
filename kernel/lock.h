// The Mordax Microkernel
// (c) Kristian Klomsten Skordal <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_LOCK_H
#define MORDAX_LOCK_H

#include "thread.h"
#include "types.h"

/**
 * @defgroup lock Lock Support
 * @{
 */

struct lock;

/** Creates a new lock. */
struct lock * lock_create(void);

/**
 * Destroys a lock.
 * @param l the lock to destroy.
 */
void lock_destroy(struct lock * l);

/**
 * Tries to aquire the specified lock.
 * @param t the thread trying to aquire the lock.
 * @param blocking set to true if the specified thread has been added to
 *                 the blocking queue.
 * @return 0 on success, or a negative error code on failure.
 */
int lock_aquire(struct lock * l, struct thread * t, bool * blocking);

/**
 * Releases a lock.
 * @param t the thread trying to release the lock.
 * @return 0 on success, or a negative error code on failure.
 */
int lock_release(struct lock * l, struct thread * t);

/** @} */

#endif

