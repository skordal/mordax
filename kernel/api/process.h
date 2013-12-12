// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_API_PROCESS_H
#define MORDAX_API_PROCESS_H

#include "types.h"

/** Mask of all the possible permissions. */
#define MORDAX_PROCESS_ALL_PERMISSIONS	0xffffffff
/** Mask indicating no permissions. */
#define MORDAX_PROCESS_NO_PERMISSIONS	0

/** Permission bit allowing a process to create new processes. */
#define MORDAX_PROCESS_PERMISSION_CREATE_PROC	(1 << 0)
/** Permission bit allowing proceccess to map random memory. */
#define MORDAX_PROCESS_PERMISSION_MMAP		(1 << 1)

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

	// Initial stack contents (such as process arguments, etc.):
	void * stack_init;		//< Pointer to where to copy the initial stack data from.
	size_t stack_init_length;	//< Length of the initial stack data.
	size_t stack_length;		//< Length of the stack to allocate for the process.

	// Initial process image data:
	void * source;			//< Data to copy into the process image.
	size_t source_length;		//< Length of the source data.

	// Section sizes:
	size_t text_length, rodata_length, data_length; //< Section size.
};

#endif

