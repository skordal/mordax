// The Mordax Standard Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <string.h>

void * memset(void * s, int c, size_t n)
{
	unsigned char * dest = s;

	for(int i = 0; i < n; ++i)
		dest[i] = c;
	return s;
}

