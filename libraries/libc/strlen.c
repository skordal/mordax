// The Mordax Standard Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <string.h>

size_t strlen(const char * s)
{
	size_t retval = 0;
	while(*s != 0)
		++s, ++retval;
	return retval;
}

