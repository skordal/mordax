// The Mordax Microkernel OS Tools
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <gelf.h>

#include <sys/types.h>
#include <sys/fcntl.h>

#include "execheader.h"

static bool write_output_loader(FILE * output_file, const char * filename);
static bool write_output_exec(FILE * output_file, const char * filename);

int main(int argc, char * argv[])
{
	FILE * output_file = NULL;
	int retval = 0;

	if(argc <= 2)
	{
		printf("mkinitproc [target loader] [application files (in ELF format)...]\n");
		return 0;
	}

	if(elf_version(EV_CURRENT) == EV_NONE)
	{
		printf("Error: cannot initialize libelf: %s\n", elf_errmsg(-1));
		retval = 1;
		goto _return;
	}

	output_file = fopen("initproc.bin", "w");
	if(output_file == NULL)
	{
		int error = errno;
		printf("Error: cannot open output file: %s\n", strerror(error));
		retval = 1;
		goto _return;
	}

	// Write the loader executable to the output file:
	if(!write_output_loader(output_file, argv[1]))
	{
		retval = 1;
		goto _return;
	}

	// Write the archive header to the output file:
	struct archive_header ahdr = { argc - 2 };
	fwrite(&ahdr, sizeof(struct archive_header), 1, output_file);

	// Write each of the application files to the output file:
	for(int i = 2; i < argc; ++i)
	{
		if(!write_output_exec(output_file, argv[i]))
		{
			retval = 1;
			goto _return;
		}
	}

_return:
	if(output_file != NULL)
		fclose(output_file);
	return retval;
}

static bool write_output_loader(FILE * output_file, const char * filename)
{
	FILE * loader_file = fopen(filename, "r");
	if(loader_file == NULL)
	{
		int error = errno;
		printf("Error: cannot open loader file %s: %s\n", filename,
			strerror(error));
		return false;
	}

	fseek(loader_file, 0, SEEK_END);
	long loader_size = ftell(loader_file);
	rewind(loader_file);

	printf("Writing loader file (%ld bytes)... ", loader_size);
	void * buffer = malloc(loader_size);
	fread(buffer, 1, loader_size, loader_file);
	fwrite(buffer, 1, loader_size, output_file);
	printf("OK\n");

	return true;
}

static bool write_output_exec(FILE * output_file, const char * filename)
{
	int input_file = open(filename, O_RDONLY);

	if(input_file == -1)
	{
		int error = errno;
		printf("Error: cannot open input file %s: %s\n", filename,
			strerror(error));
		return false;
	}

	Elf * input_elf = elf_begin(input_file, ELF_C_READ, NULL);
	if(input_elf == NULL)
	{
		printf("Error: could not open ELF file %s: %s\n", filename,
			elf_errmsg(-1));
		close(input_file);
		return false;
	}

	if(elf_kind(input_elf) != ELF_K_ELF)
	{
		printf("Error: input file %s if not an ELF object\n", filename);
		close(input_file);
		return false;
	}

	size_t shdr_string_table_index;
	if(elf_getshdrstrndx(input_elf, &shdr_string_table_index))
	{
		printf("Error: could not get section header string table index: %s\n",
			elf_errmsg(-1));
		close(input_file);
		return false;
	}

	// Make room for the header, which is written after the actual executable data:
	long header_location = ftell(output_file);
	fseek(output_file, sizeof(struct exec_header), SEEK_CUR);

	struct exec_header header;
	memset(&header, 0, sizeof(struct exec_header));

	char * basename_temp = strdup(filename);
	char * invocation_name = basename(basename_temp);
	strncpy(header.name, invocation_name, 16);
	header.name[15] = 0;
	free(basename_temp);

	uint32_t offset = 0; // Offset in the output file from the executable header.
	Elf_Scn * section = NULL;
	while((section = elf_nextscn(input_elf, section)) != NULL)
	{
		GElf_Shdr section_header;
		gelf_getshdr(section, &section_header);

		const char * section_name = elf_strptr(input_elf, shdr_string_table_index,
			section_header.sh_name);
		printf("Section: %s\n", section_name);
		printf("\tsize: %d\n", section_header.sh_size);
		printf("\tstart: %#x\n", section_header.sh_addr);
		printf("\toffset: %d\n", offset);

		if(!strcmp(section_name, ".text"))
		{
			header.text_size = section_header.sh_size;
			header.text_start = offset;
		} else if(!strcmp(section_name, ".data"))
		{
			header.data_size = section_header.sh_size;
			header.data_start = offset;
		} else if(!strcmp(section_name, ".rodata"))
		{
			header.rodata_size = section_header.sh_size;
			header.rodata_start = offset;
		} else if(!strcmp(section_name, ".bss"))
		{
			header.bss_size = section_header.sh_size;
			continue;
		} else {
			printf("\tsection ignored\n");
			continue;
		}

		char * buffer = malloc(section_header.sh_size);
		int copied = 0;

		Elf_Data * data_source = NULL;
		while(copied < section_header.sh_size && (data_source = elf_getdata(section, data_source)) != NULL)
		{
			memcpy(buffer + copied, data_source->d_buf, data_source->d_size);
			copied += data_source->d_size;
		}

		fwrite(buffer, 1, section_header.sh_size, output_file);
		offset += section_header.sh_size;
		free(buffer);
	}

	elf_end(input_elf);
	close(input_file);

	long end_position = ftell(output_file);
	fseek(output_file, header_location, SEEK_SET);
	fwrite(&header, 1, sizeof(struct exec_header), output_file);
	fseek(output_file, end_position, SEEK_SET);

	return true;
}

