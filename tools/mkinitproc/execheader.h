// The Mordax Microkernel OS Tools
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MKINITPROC_EXECHEADER_H
#define MKINITPROC_EXECHEADER_H

#include <stdint.h>

struct exec_header
{
	// Section sizes:
	uint32_t text_size, rodata_size, data_size, bss_size;
	// Offset from the header that the data for a section is found:
	uint32_t text_start, rodata_start, data_start;
	// Name that the process is invoked by:
	char name[16];
} __attribute((packed));

struct archive_header
{
	uint32_t num_files;
} __attribute((packed));

#endif

