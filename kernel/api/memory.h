// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_API_MEMORY_H
#define MORDAX_API_MEMORY_H

#include "types.h"

/**
 * Memory area types.
 */
enum mordax_memory_type
{
	MORDAX_TYPE_CODE,
	MORDAX_TYPE_RODATA,
	MORDAX_TYPE_DATA,
	MORDAX_TYPE_STACK,
	MORDAX_TYPE_STRORD,
	MORDAX_TYPE_DEVICE,
	MORDAX_TYPE_UNCACHED
};

/**
 * Memory permissions.
 */
enum mordax_memory_permissions
{
	MORDAX_PERM_RW_RW,
	MORDAX_PERM_RW_RO,
	MORDAX_PERM_RW_NA,
	MORDAX_PERM_RO_RO,
	MORDAX_PERM_RO_NA,
	MORDAX_PERM_NA_NA,
};

/**
 * Structure describing the attributes of a memory area.
 */
struct mordax_memory_attributes
{
	enum mordax_memory_type type;
	enum mordax_memory_permissions permissions;
};

/**
 * Structure describing a zone of memory.
 */
struct mordax_memory_zone
{
	void * base;					//< Base address of the memory zone
	size_t size;					//< Size of the memory zone
	struct mordax_memory_attributes attributes;	//< Attributes of the memory zone
};

/**
 * Structure used for describing the initial memory map of a process.
 */
struct mordax_memory_map
{
	int num_zones;				//< Number of memory zones in the memory map.
	struct mordax_memory_zone * zones;	//< Pointer to an array of the memory zones.
};

#endif

