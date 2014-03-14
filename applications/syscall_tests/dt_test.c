// The Mordax Microkernel OS Device Tree API Test Programme
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <mordax.h>
#include <mordax-dt.h>

int main(void)
{
	mordax_system(MORDAX_SYSTEM_DEBUG, "Device tree test application");
 
 	// Open the device tree node at /
	mordax_system(MORDAX_SYSTEM_DEBUG, "Attempting to open device tree node /...\n");
	mordax_resource_t root_node = mordax_dt_get_node_by_path("/", 1);
	if(root_node < 0)
		mordax_system(MORDAX_SYSTEM_DEBUG, "Failed!");
	else
		mordax_system(MORDAX_SYSTEM_DEBUG, "Success!");

	// Get the value of the "compatible" property of the root node:
	struct mordax_dt_string name = { "compatible", 10 };
	char buffer[32];
	struct mordax_dt_string retval = { buffer, 32 };
	mordax_system(MORDAX_SYSTEM_DEBUG, "Attempting to get the value of /compatible...");
	int status = mordax_dt_get_property_string(root_node, &name, &retval);
	if(status != 0)
		mordax_system(MORDAX_SYSTEM_DEBUG, "Failed!");
	else
		mordax_system(MORDAX_SYSTEM_DEBUG, retval.string);

	mordax_resource_destroy(root_node);
}

