// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 - 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_API_PROCESS_H
#define MORDAX_API_PROCESS_H

#include "types.h"

/** Mask of all the possible permissions. */
#define MORDAX_PROCESS_ALL_PERMISSIONS		0x7fffffff
/** Mask indicating no permissions. */
#define MORDAX_PROCESS_NO_PERMISSIONS		0

/** Permission bit allowing a process to create new processes. */
#define MORDAX_PROCESS_PERMISSION_CREATE_PROC	(1 << 0)
/** Permission bit allowing processes to map random memory. */
#define MORDAX_PROCESS_PERMISSION_MAP_MEMORY	(1 << 1)
/** Permission bit allowing processes to create and manage services. */
#define MORDAX_PROCESS_PERMISSION_SERVICE	(1 << 2)
/** Permission bit allowing processes to create and manage locks. */
#define MORDAX_PROCESS_PERMISSION_LOCKS		(1 << 3)
/** Permission bit allowing processes to use IRQ objects. */
#define MORDAX_PROCESS_PERMISSION_IRQ		(1 << 4)

/**
 * Permission bit specifying that all permissions should be inherited from
 * the parent process. All other permission bits are ignored if this is set.
 */
#define MORDAX_PROCESS_INHERIT_PERMISSIONS	(1 << 31)

/** Stack length value specifying that the stack size should be the same as the parent. */
#define MORDAX_PROCESS_INHERIT_STACK_SIZE	0xffffffff

/**
 * Structure describing a new process.
 * @note The section sizes must be multiples of the page size, and
 *       they must add up to be equal to the size of the process
 *       image (`source_length`) rounded up to the next page
 *       boundary.
 */
struct mordax_process_info
{
	gid_t gid;			//< GID of the new process.
	uid_t uid;			//< UID of the new process.
	uint32_t permissions;		//< Permissions of the new process.

	// Details for the initial thread of the new process:
	void * entry_point;		//< Entry point for the application.

	// Section sizes:
	size_t text_length, rodata_length, data_length, stack_length; //< Section sizes.

	// Initial process image data:
	void * text_source, * rodata_source, * data_source, * stack_source;
	size_t text_source_length, rodata_source_length, data_source_length, stack_source_length;
};

#endif

