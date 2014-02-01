// The Mordax Standard Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <string.h>

void * memcpy(void * dest, const void * src, size_t n)
{
	unsigned char * d = dest;
	const unsigned char * s = src;

	for(int i = 0; i < n; ++i)
		d[i] = s[i];
	return dest;
}


