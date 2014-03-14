// The Mordax System Call Library
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef __MORDAX_DT_H__
#define __MORDAX_DT_H__

#include <mordax/dt.h>
#include <mordax/types.h>

/**
 * Looks up a device tree node by path.
 * @param path full path of the device tree node.
 * @param path_length length of the path, excluding the terminating NULL character.
 * @return a resource identifier for the device tree node or a negative error code.
 */
mordax_resource_t mordax_dt_get_node_by_path(const char * path, size_t path_length);

/**
 * Looks up a device tree node by its phandle.
 * @param phandle the phandle to look up.
 * @return a resource identifier for the device tree node or a negative error code.
 */
mordax_resource_t mordax_dt_get_node_by_phandle(unsigned int phandle);

/**
 * Gets an array of 32-bit values from a device tree node.
 * @param identifier resource identifier for the node.
 * @param name name of the property to read data from.
 * @param out pointer to where the result should be stored.
 * @param length number of values to read.
 * @return 0 on success, a negative error code on error.
 */
int mordax_dt_get_property_array32(mordax_resource_t identifier, struct mordax_dt_string * name,
	uint32_t * out, size_t length);

/**
 * Gets a string property from a device tree node.
 * @param identifier resource identifier for the node.
 * @param name name of the property to read data from.
 * @param out pointer to a `struct mordax_dt_string` with a preallocated string member where
 *            the resulting string can be stored. The length member should be set to the length
 *            of the string buffer. It is updated after a successful call to this function.
 * @return 0 on success, a negative error code on error.
 */
int mordax_dt_get_property_string(mordax_resource_t identifier, struct mordax_dt_string * name,
	struct mordax_dt_string * out);

/**
 * Gets a phandle property from a device tree node.
 * @param identifier resource identifier for the node.
 * @param name name of the property.
 * @param out pointer to where the result is stored.
 * @return 0 on success, a negative error code on error.
 */
int mordax_dt_get_property_phandle(mordax_resource_t identifier, struct mordax_dt_string * name,
	unsigned int * out);

#endif

