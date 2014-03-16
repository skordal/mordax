// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_DT_H
#define MORDAX_DT_H

#include <stdbool.h>
#include "api/types.h"

/**
 * @defgroup dt Device Tree Support
 * @{
 */

/** Flattened device tree header type. */
struct fdt;

struct dt
{
	struct dt_node * root;
};

struct dt_node
{
	char * name;
	struct dt_property * properties;

	struct dt_node * next;
	struct dt_node * children;
};

struct dt_property
{
	char * name;
	struct dt_property * next;

	uint32_t value_length;
	uint8_t * value;
};

typedef uint32_t dt_phandle;

/**
 * Creates a device tree structure from the flattened device tree passed by
 * U-Boot.
 * @param fdt the flattened device tree from U-Boot.
 * @return a device tree structure or 0 if an error occured.
 */
struct dt * dt_parse(struct fdt * fdt);

/**
 * Gets a device tree node by path. `/' is the root node.
 * @param dt the device tree to search in.
 * @param path the path of the device tree node to retrieve.
 * @return the specified device tree node or 0 if it was not found.
 */
struct dt_node * dt_get_node_by_path(struct dt * dt, const char * path);

/**
 * Gets a device tree node by phandle.
 * @param dt the device tree to search in.
 * @param phandle the phandle to look for.
 * @return the device tree node or 0 if it was not found.
 */
struct dt_node * dt_get_node_by_phandle(struct dt * dt, dt_phandle phandle);

/**
 * Gets a device tree node by its alias. The /aliases node must exist for this
 * to work.
 * @param dt the device tree to search in.
 * @param alias the alias name to look for.
 * @return the device tree node or 0 if it was not found.
 */
struct dt_node * dt_get_node_by_alias(struct dt * dt, const char * alias);

/**
 * Gets a device tree node by its `compatible` value.
 * @param dt the device tree to search in.
 * @param compatible value of the compatible string. If there are multiple
 *                   values listed in the device tree, these will be searched
 *                   separately.
 * @param index index into the list of search results of the node to retrieve.
 * @return the device tree node or 0 if no node was found or no node was found
 *         at the specified index
 */
struct dt_node * dt_get_node_by_compatible(struct dt * dt, const char * compatible,
	int index);

/**
 * Gets the specified subnode of a device tree node.
 * @param node the node to search the children of.
 * @param name name of the node to find.
 * @return the specified device tree node or 0 if no node was found.
 */
struct dt_node * dt_get_subnode(struct dt_node * node, const char * name);

/**
 * Checks if a property exists in a device tree node.
 * @param node the node to check.
 * @param name name of the property to look for.
 * @return `true` if the property exists, `false` otherwise.
 */
bool dt_property_exists(struct dt_node * node, const char * name);

/**
 * Gets a string property from a device tree node.
 * @param node the node to retrieve the property from.
 * @param name property name.
 * @return the value of the string property or 0 if no property was found.
 */
const char * dt_get_string_property(struct dt_node * node, const char * name);

/**
 * Gets an array of 32-bit values from a device tree node property.
 * The values are converted from big endian to little endian before being returned.
 * @param node the node to retrieve the property values from.
 * @param name property name.
 * @param out a preallocated array to return the values in.
 * @param length length of the array to return.
 * @return `true` if successful, `false` otherwise.
 */
bool dt_get_array32_property(struct dt_node * node, const char * name, uint32_t * out, size_t length);

/**
 * Gets a phandle property from a device tree node.
 * @param node the node to retrieve the property value from.
 * @param name property name.
 * @return the phandle from the property.
 */
dt_phandle dt_get_phandle_property(struct dt_node * node, const char * name);

/**
 * Prints the contents of a device tree in a pretty fashion.
 * @param dt the device tree to print.
 */
void dt_print(struct dt * dt);

/** @} */

#endif

