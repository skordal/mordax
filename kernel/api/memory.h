// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_API_MEMORY_H
#define MORDAX_API_MEMORY_H

#include <types.h>

/**
 * Memory area types.
 */
enum mordax_memory_type
{
	MMU_TYPE_CODE,
	MMU_TYPE_RODATA,
	MMU_TYPE_DATA,
	MMU_TYPE_STACK,
	MMU_TYPE_STRORD,
	MMU_TYPE_DEVICE,
	MMU_TYPE_UNCACHED
};

/**
 * Memory permissions.
 */
enum mordax_memory_permissions
{
	MMU_PERM_RW_RW,
	MMU_PERM_RW_RO,
	MMU_PERM_RW_NA,
	MMU_PERM_RO_RO,
	MMU_PERM_RO_NA,
	MMU_PERM_NA_NA,
};

/**
 * Structure describing a zone of memory.
 */
struct mordax_memory_zone
{
	void * base;					//< Base address of the memory zone
	size_t size;					//< Size of the memory zone
	enum mordax_memory_type type;			//< Type of the memory zone
	enum mordax_memory_permissions permissions;	//< Permissions of the memory zone
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

