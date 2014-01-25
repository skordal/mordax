// The Mordax Operating System Common Modules Library
// (c) Kristian Klomsten Skordal <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_RBTREE_H
#define MORDAX_RBTREE_H

#include <stdbool.h>

/**
 * @defgroup rbtree Red-Black Tree Functions
 * Functionality for working with red-black trees.
 *
 * The red-black tree is implemented using algorithms from
 * "Introduction to Algorithms" by Cormen, et al.
 *
 * This implementation is based on the implementation available on
 * https://github.com/skordal/librbtree
 *
 * @{
 */

/** Abstract red-black tree structure. */
struct rbtree;

/**
 * Key comparison type. Compares two keys, returning -1 if a < b,
 * 0 if a = b and 1 if a > b.
 */
typedef int (*rbtree_key_compare_func)(const void *, const void *);
/** Key duplication function type. */
typedef void * (*rbtree_key_duplicate_func)(const void *);
/** Key deallocation function type. */
typedef void (*rbtree_key_free_func)(void *);
/** Data deallocation function type. */
typedef void (*rbtree_data_free_func)(void *);

/**
 * Constructs an empty red-black tree. The arguments to this function can all be `0`.
 * @param key_compare function for comparing keys.
 * @param key_free function for freeing keys.
 * @param key_dup function for duplicating keys.
 * @param data_free function for freeing node data.
 * @return a new, empty red-black tree.
 */
struct rbtree * rbtree_new(rbtree_key_compare_func key_compare,
	rbtree_key_free_func key_free, rbtree_key_duplicate_func key_dup,
	rbtree_data_free_func data_free) __attribute((malloc));

/**
 * Frees a red-black tree and all its nodes.
 * Can also optionally call a function to free the data stored in the nodes.
 * @param tree the tree to free.
 * @param key_free_func the function to use for freeing the keys in nodes or 0
 *                      if the keys should not be freed or freed using the
 *                      free function previously assigned to the tree.
 * @param data_free_func the function to use for freeing data in nodes or 0 if the
 *                  data should not be freed.
 */
void rbtree_free(struct rbtree * tree);

/**
 * Inserts a node in a red-black tree.
 * @param tree the tree to insert into.
 * @param key the key to insert at.
 * @param data the data to insert into the tree.
 */
void rbtree_insert(struct rbtree * tree, const void * key, void * data);

/**
 * Deletes a node in a red-black tree.
 * @param tree the tree to delete from.
 * @param key the key to delete.
 * @return the data associated with the deleted key, or 0 if the key did not
 *         exist.
 */
void * rbtree_delete(struct rbtree * tree, const void * key);

/**
 * Gets the data associated with a specified key in a red-black tree.
 * @param tree the tree to do the lookup in.
 * @param key the key to look for.
 * @return the data associated with the key, or 0 if the key was not found.
 */
void * rbtree_get_value(struct rbtree * tree, const void * key);

/**
 * Checks if a specified key exists in a red-black tree.
 * @param tree the tree to look in.
 * @param key the key to check for.
 * @return `true` if the key is in the tree, `false` otherwise.
 */
bool rbtree_key_exists(struct rbtree * tree, const void * key);

/** @} */

#endif

