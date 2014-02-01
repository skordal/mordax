// The Mordax Microkernel OS Tools
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <mordax.h>
#include "execheader.h"

extern void * archive_start;

// Utility functions used:
size_t strlen(const char * s);
void * memset(void * restrict memory, char value, size_t length);
void * memcpy(void * dest, const void * src, size_t length);

void loader(void)
{
	mordax_system(MORDAX_SYSTEM_DEBUG, "Mordax loader running.");

	// Get the archive header:
	const struct archive_header * arch_hdr = (struct archive_header *) &archive_start;
	// Get the first executable header:
	struct exec_header * exe_header = (void *) ((char *) &archive_start) + sizeof(struct archive_header);

	// Iterate through the executable files:
	for(int i = 0; i < arch_hdr->num_files; ++i)
	{
		mordax_system(MORDAX_SYSTEM_DEBUG, exe_header->name);

		struct mordax_process_info process_info = {
			.permissions = MORDAX_PROCESS_ALL_PERMISSIONS,
			.entry_point = (void *) 0x1000,
			.text_length = (exe_header->text_size + 4095) & -4096,
			.data_length = (exe_header->data_size + exe_header->bss_size + 4095) & -4096,
			.rodata_length = (exe_header->rodata_size + 4095) & -4096,
			.stack_length = MORDAX_PROCESS_INHERIT_STACK_SIZE,
			.text_source_length = exe_header->text_size,
			.data_source_length = exe_header->data_size,
			.rodata_source_length = exe_header->rodata_size,
		};

		if(exe_header->text_size > 0)
			process_info.text_source = (void *) ((char *) exe_header + sizeof(struct exec_header) + exe_header->text_start);
		if(exe_header->rodata_size > 0)
			process_info.rodata_source = (void *) ((char *) exe_header + sizeof(struct exec_header) + exe_header->rodata_start);
		if(exe_header->data_size > 0)
			process_info.data_source = (void *) ((char *) exe_header + sizeof(struct exec_header) + exe_header->data_start);

		mordax_process_create(&process_info);
//		mordax_thread_yield();

		// Calculate the location of the next executable header:
		const int next_hdr_offset = exe_header->text_size + exe_header->data_size + exe_header->rodata_size;
		exe_header = (void *) (((char *) exe_header) + sizeof(struct exec_header) + next_hdr_offset);
	}

	// Exit the loader application:
	mordax_thread_exit(0);
}

/* --- Utility functions: --- */

size_t strlen(const char * s)
{
	int retval = 0;
	for(; s[retval] != 0; ++retval);
	return retval;
}

void * memset(void * restrict memory, char value, size_t length)
{
	char * s = memory;
	for(int i = 0; i < length; ++i)
		s[i] = value;
	return memory;
}

void * memcpy(void * dest, const void * src, size_t length)
{
	const char * s = src;
	char * d = dest;

	for(int i = 0; i < length; ++i)
		d[i] = s[i];
	return dest;
}

