// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "dt.h"
#include "irq.h"
#include "kernel.h"
#include "mm.h"
#include "mmu.h"
#include "rbtree.h"

#include "debug.h"
#include "drivers/debug/debug.h"

// Kernel device tree:
static struct dt * kernel_dt;

// Functions exported from target specific code:
extern void interrupts_initialize(void);
extern void stacks_initialize(void);

// Initialization functions:
static void initialize_debug_uart(struct dt_node * mordax_node);
static void initialize_intc(struct dt_node * mordax_node);
static void load_and_start_initproc(void) __attribute((noreturn));

void kernel_main(physical_ptr * device_tree, size_t dt_size)
{
	extern void * load_address; // Physical load address, exported from the linker script.
	extern void * kernel_address; // Virtual load address, exported from the linker script.

	// Initialize basic memory management functions:
	mm_initialize();
	mmu_initialize();

	// Print the debug console header:
	debug_printf("The Mordax Microkernel v0.1\n");
	debug_printf("(c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>\n");
	debug_printf("Report bugs and issues on <http://github.com/skordal/mordax/issues>\n\n");

	// Remap the interrupt handlers:
	debug_printf("Installing interrupt handlers... ");
	interrupts_initialize();
	debug_printf("finished\n");

	// Map and parse the FDT passed from uboot and create a more accessible local version of it:
	debug_printf("Parsing FDT... ");
	struct fdt * uboot_fdt = mmu_map(device_tree, device_tree, dt_size, MMU_TYPE_DATA, MMU_PERM_RW_NA);
	kernel_dt = dt_parse(uboot_fdt);
	if(kernel_dt == 0)
		kernel_panic("could not parse the device tree");
	mmu_unmap(device_tree, dt_size);

	debug_printf("Device tree:\n");
	dt_print(kernel_dt);

	debug_printf("Hardware: %s (compatible: %s)\n",
		dt_get_string_property(kernel_dt->root, "model"),
		dt_get_string_property(kernel_dt->root, "compatible"));

	// Set up physical memory management:
	debug_printf("Memory:\n");
	struct dt_node * memory_node = dt_get_node_by_path(kernel_dt, "/memory");
	uint32_t memory_data[2];
	if(!dt_get_array32_property(memory_node, "reg", memory_data, 2))
		kernel_panic("the 'memory' node is missing the 'reg' property");
	debug_printf("\t%d bytes @ %x\n", memory_data[1], memory_data[0]);
	mm_add_physical((physical_ptr) memory_data[0], (size_t) memory_data[1], MM_ZONE_NORMAL);
	
	// Reserve the memory currently in use from being allocated:
	mm_reserve_physical(&load_address, (uint32_t) kernel_dataspace_end - (uint32_t) &kernel_address);

	// Set up required stacks:
	stacks_initialize();

	// Get the Mordax node from the device tree:
	struct dt_node * mordax_node = dt_get_node_by_path(kernel_dt, "/mordax");
	if(mordax_node == 0)
		kernel_panic("the 'mordax' node is missing from the device tree");

	// Initialize the debug driver:
	initialize_debug_uart(mordax_node);

	// Initialize the interrupt controller driver:
	initialize_intc(mordax_node);

	// Load the initial process into memory:
	load_and_start_initproc();
}

void kernel_panic(const char * message)
{
	debug_printf("*** KERNEL PANIC\n");
	debug_printf("*** ERROR: %s\n\n", message);
	debug_printf("*** An unrecoverable error has occured. Reset the board and try again.\n");

	while(1) asm volatile("cpsid aif\n\twfi\n\t");
}

static void initialize_debug_uart(struct dt_node * mordax_node)
{
	struct dt_node * uart_node = 0;
	dt_phandle uart_phandle;

	uart_phandle = dt_get_phandle_property(mordax_node, "debug-interface");
	if(uart_phandle == 0)
		debug_printf("Warning: no debug-interface under /mordax in the device tree");
	else {
		uart_node = dt_get_node_by_phandle(kernel_dt, uart_phandle);
		debug_set_output_driver(debug_driver_instantiate(uart_node));
	}
}

static void initialize_intc(struct dt_node * mordax_node)
{
	struct dt_node * intc_node = 0;
	dt_phandle intc_phandle;

	intc_phandle = dt_get_phandle_property(mordax_node, "interrupt-controller");
	if(intc_phandle == 0)
		kernel_panic("no interrupt-controller node under /mordax in the device tree");

	intc_node = dt_get_node_by_phandle(kernel_dt, intc_phandle);
	irq_set_intc_driver(intc_driver_instantiate(intc_node));
}

static void load_and_start_initproc(void)
{
	struct dt_node * chosen_node = dt_get_node_by_path(kernel_dt, "/chosen");
	uint32_t initproc_start, initproc_end;

	if(!dt_get_array32_property(chosen_node, "linux,initrd-start", &initproc_start, 1))
		kernel_panic("could not get start address of the initial process");
	if(!dt_get_array32_property(chosen_node, "linux,initrd-end", &initproc_end, 1))
		kernel_panic("could not get end address of the initial process");
	debug_printf("Initial process at %x-%x physical\n", initproc_start, initproc_end);

	// Wait for the first scheduler interrupt to arrive:
	debug_printf("Initialization finished.\n");
	while(true) asm volatile("wfi\n\t");
}

