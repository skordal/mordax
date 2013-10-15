// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_RBTREE_H
#define MORDAX_RBTREE_H

#include "types.h"

/**
 * @defgroup rbtree Red-Black Tree Functions
 * Functionality for working with red-black trees.
 *
 * The red-black tree is implemented using algorithms and guidance from
 * "Introduction to Algorithms" by Cormen, et al. Turns out finding other
 * sources for information on red-black trees is difficult, as everyone
 * else on the internet seems to use Cormen as well.
 *
 * @todo Add support for removing entries from the tree.
 * @todo Rewrite the whole thing so that it has no bugs.
 * @{
 */

/** Abstract red-black tree structure. */
struct rbtree;

/**
 * Allocates a new, empty red-black tree.
 * @return a pointer to an empty red-black tree.
 */
struct rbtree * rbtree_new(void) __attribute((malloc));

/**
 * Frees a red-black tree and all its nodes.
 * @param tree the tree to free.
 * @todo Add callback for deleting the node data of each freed node.
 */
void rbtree_free(struct rbtree * tree);

/**
 * Inserts an element into a red-black tree.
 * @param tree the tree to insert into.
 * @param key the key to insert the element for.
 * @param data the data to insert into the tree.
 */
void rbtree_insert(struct rbtree * tree, uint32_t key, void * data);

/**
 * Removes an element from a red-black tree.
 * @param tree the tree to remove from.
 * @param key the key to delete from the tree.
 * @return the data associated with the removed key or 0 if nothing was removed.
 */
void * rbtree_remove(struct rbtree * tree, uint32_t key);

/**
 * Gets the value associated with a specified key.
 * @param tree the tree to use for looking up the key.
 * @param key the key to look up.
 * @return the data associated with the key, or 0 if the key was not found.
 */
void * rbtree_get_value(struct rbtree * tree, uint32_t key);

/**
 * Checks if a key exists in a tree.
 * @param tree the tree to search in.
 * @param key the key to find.
 * @return `true` if the key exists in the tree, `false` otherwise.
 */
bool rbtree_key_exists(struct rbtree * tree, uint32_t key);

/** @} */

#endif

