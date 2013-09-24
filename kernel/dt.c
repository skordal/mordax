// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "dt.h"
#include "mm.h"
#include "utils.h"

#define FDT_MAGIC	0xd00dfeed

#define FDT_BEGIN_NODE	1
#define FDT_END_NODE	2
#define FDT_PROP	3
#define FDT_END		9

// Flattened device tree header fields:
struct fdt
{
	be32 magic;
	be32 size;
	be32 struct_offset;
	be32 strings_offset;
	be32 rsvmap_offset;
	be32 version;
};

// Prints a device tree node and all its properties and child nodes:
static void dt_print_node(struct dt_node * node, int indentation_level);

// Parses the FDT node pointed to by the first argument, storing the result
// in ret and returning the number of 32-bit words consumed:
static uint32_t dt_parse_node(struct fdt *, be32 * fdt_node, struct dt_node ** ret);

// Parses a property from the property list for a node and returns the number
// of 32-bit words consumed:
static uint32_t dt_parse_property(struct fdt *, be32 * prop, struct dt_property ** p);

// Gets a pointer to a string in the string table:
static const char * dt_get_string(struct fdt * fdt, uint32_t offset);

struct dt * dt_parse(struct fdt * fdt)
{
	// Check for a valid flattened device tree:
	if(be2le32(*((be32 *) fdt)) != FDT_MAGIC)
		return 0;

	struct dt * retval = mm_allocate(sizeof(struct dt), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	uint32_t * structure_block = (uint32_t *) ((uint32_t) fdt + be2le32(fdt->struct_offset));

	// Parse the root node:
	struct dt_node * root_node = 0;
	dt_parse_node(fdt, structure_block, &root_node);

	retval->root = root_node;
	return retval;
}

struct dt_node * dt_get_node_by_path(struct dt * dt, const char * path)
{
	if(path[0] != '/' || dt == 0)
		return 0;	

	char * nextpath = mm_allocate(strlen(path + 1) + 1, MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	char * free_nextpath = nextpath;
	strcpy(nextpath, path + 1);

	struct dt_node * retval = dt->root;
	char * next_node_name = strsep(&nextpath, '/');
	do {
		retval = dt_get_subnode(retval, next_node_name);
		next_node_name = strsep(&nextpath, '/');
	} while(nextpath != 0);

	mm_free(free_nextpath);
	return retval;
}

struct dt_node * dt_get_node_by_phandle(struct dt * dt, dt_phandle phandle)
{
	struct dt_node * retval = dt->root;
	do {
		dt_phandle current_phandle = 0;

		if(dt_property_exists(retval, "linux,phandle"))
			current_phandle = dt_get_phandle_property(retval, "linux,phandle");
		else if(dt_property_exists(retval, "phandle"))
			current_phandle = dt_get_phandle_property(retval, "phandle");

		if(current_phandle == phandle)
			return retval;

		if(retval->next)
			retval = retval->next;
		else
			retval = retval->children;
	} while(retval != 0);

	return 0;
}

struct dt_node * dt_get_subnode(struct dt_node * node, const char * name)
{
	for(struct dt_node * child = node->children; child != 0; child = child->next)
		if(str_equals(child->name, name))
			return child;
	return 0;
}

bool dt_property_exists(struct dt_node * node, const char * name)
{
	for(struct dt_property * current = node->properties; current != 0; current = current->next)
		if(str_equals(current->name, name))
			return true;
	return false;
}

const char * dt_get_string_property(struct dt_node * node, const char * name)
{
	for(struct dt_property * current = node->properties; current != 0; current = current->next)
		if(str_equals(current->name, name))
			return (const char *) current->value;
	return 0;
}

bool dt_get_array32_property(struct dt_node * node, const char * name, uint32_t out[], size_t length)
{
	for(struct dt_property * current = node->properties; current != 0; current = current->next)
	{
		if(str_equals(current->name, name))
		{
			be32 * values = (be32 *) current->value;
			for(unsigned i = 0; i < length; ++i)
				out[i] = be2le32(values[i]);
			return true;
		}
	}

	return false;
}

dt_phandle dt_get_phandle_property(struct dt_node * node, const char * name)
{
	dt_phandle retval = 0;
	dt_get_array32_property(node, name, &retval, 1);
	return retval;
}

void dt_print(struct dt * dt)
{
	dt_print_node(dt->root, 0);
}

static void dt_print_node(struct dt_node * node, int indentation_level)
{
	for(int i = 0; i < indentation_level; ++i)
		debug_printf("\t");
	debug_printf("Node name: %s\n", node->name);

	for(struct dt_property * property = node->properties; property != 0; property = property->next)
	{
		for(int i = 0; i < indentation_level; ++i)
			debug_printf("\t");
		debug_printf("\t%s\n", property->name);
	}

	for(struct dt_node * child = node->children; child != 0; child = child->next)
		dt_print_node(child, indentation_level + 1);
}

static uint32_t dt_parse_node(struct fdt * fdt, be32 * fdt_node, struct dt_node ** ret)
{
	uint32_t retval = 0;
	*ret = mm_allocate(sizeof(struct dt_node), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	memclr(*ret, sizeof(struct dt_node));

	if(be2le32(*fdt_node) != FDT_BEGIN_NODE)
	{
		*ret = 0;
		return 0;
	}

	++retval; ++fdt_node;
	(*ret)->name = mm_allocate(strlen((const char *) fdt_node) + 1,
		MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	strcpy((*ret)->name, (const char *) fdt_node);

	retval += ((strlen((*ret)->name) + 4) & -4) >> 2;
	fdt_node += ((strlen((*ret)->name) + 4) & -4) >> 2;

	// Parse the property list:
	struct dt_property * last_prop = (*ret)->properties;
	while(be2le32(*fdt_node) == FDT_PROP)
	{
		struct dt_property * property;
		uint32_t inc = dt_parse_property(fdt, fdt_node, &property);

		if(last_prop == 0)
			(*ret)->properties = property;
		else
			last_prop->next = property;
		last_prop = property;

		retval += inc; fdt_node += inc;
	}

	// Parse any child nodes:
	struct dt_node * last_child = (*ret)->children;
	while(be2le32(*fdt_node) == FDT_BEGIN_NODE)
	{
		struct dt_node * child = 0;
		uint32_t inc = dt_parse_node(fdt, fdt_node, &child);

		if(last_child == 0)
			(*ret)->children = child;
		else
			last_child->next = child;
		last_child = child;

		retval += inc; fdt_node += inc;
	}

	++retval; ++fdt_node;
	return retval;
}

static uint32_t dt_parse_property(struct fdt * fdt, be32 * prop, struct dt_property ** p)
{
	uint32_t retval = 0;

	if(be2le32(*prop) != FDT_PROP)
	{
		*p = 0;
		return 0;
	}

	*p = mm_allocate(sizeof(struct dt_property), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	memclr(*p, sizeof(struct dt_property));

	// Get the length of the property value:
	++retval; ++prop;
	(*p)->value_length = be2le32(*prop);

	// Get the name of the property:
	++retval; ++prop;
	(*p)->name = mm_allocate(strlen(dt_get_string(fdt, be2le32(*prop))) + 1, MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	strcpy((*p)->name, dt_get_string(fdt, be2le32(*prop)));

	// Copy out the value of the property:
	++retval; ++prop;
	if((*p)->value_length != 0)
	{
		(*p)->value = mm_allocate((*p)->value_length, MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
		memcpy((*p)->value, prop, (*p)->value_length);
		retval += (((*p)->value_length + 3) & -4) >> 2;
	}

	return retval;
}

static const char * dt_get_string(struct fdt * fdt, uint32_t offset)
{
	const char * string_section = (void *) ((uint32_t) fdt + be2le32(fdt->strings_offset));
	return string_section + offset;
}

