// The Mordax Microkernel OS Memory Mapping Test Programme
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <stdint.h>
#include <mordax.h>

// Size of the application image:
extern void * image_size;

// NOTE: This test application only works on the BeagleBoard, as it uses the UART.

int main(void)
{
	mordax_system(MORDAX_SYSTEM_DEBUG, "Mordax Developer Test Application");

	// Test mapping the UART and printing a simple message:
	struct mordax_memory_attributes attributes = {
		.type = MORDAX_TYPE_DEVICE,
		.permissions = MORDAX_PERM_RW_RW
	};
	volatile char * uart = mordax_memory_map((void *) 0x20000000, (void *) 0x49020000,
		4096, &attributes);
	if((uint32_t) uart == 0x20000000)
		mordax_system(MORDAX_SYSTEM_DEBUG, "Memory mapping succeeded");
	else {
		mordax_system(MORDAX_SYSTEM_DEBUG, "Memory mapping failed");
		return 1;
	}

	// Test the mapped memory:
	char * c = "test string\n\r";
	for(int i = 0; i < 13; ++i)
	{
		while(!(uart[0x14] & (1 << 5)));
		*uart = c[i];
	}

	mordax_system(MORDAX_SYSTEM_DEBUG, "Unmapping memory...");
	mordax_memory_unmap((void *) uart, 4096);

	// Test allocating memory:
	attributes.type = MORDAX_TYPE_DATA;
	size_t size = 4096;
	uint32_t * testmem = mordax_memory_map_alloc((void *) 0x20000000, &size,
		&attributes);
	if(testmem == 0)
	{
		mordax_system(MORDAX_SYSTEM_DEBUG, "Memory mapping failed");
		return 1;
	} else
		mordax_system(MORDAX_SYSTEM_DEBUG, "Memory mapping succeeded");

	// Test the mapped memory:
	for(int i = 0; i < 13; ++i)
		testmem[i] = c[i];
/*	for(int i = 0; i < 13; ++i)
	{
		if(testmem[i] != c[i])
		{
			mordax_system(MORDAX_SYSTEM_DEBUG, "Error while verifying mapped memory:");
			mordax_system(MORDAX_SYSTEM_DEBUG, "written data differs from read data");
			return 1;
		}
	}
*/
	mordax_system(MORDAX_SYSTEM_DEBUG, "Unmapping allocated memory...");
	mordax_memory_unmap((void *) testmem, size);

	mordax_system(MORDAX_SYSTEM_DEBUG, "Finished.");
	return 0;
}

