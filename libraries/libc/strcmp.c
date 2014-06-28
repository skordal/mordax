// The Mordax Standard Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <string.h>

int strcmp(const char * s1, const char * s2)
{
	while(*s1 != 0 && *s1 == *s2)
	{
		++s1;
		++s2;
	}

	return (unsigned char) *s1 - (unsigned char) *s2;
}

int strncmp(const char * s1, const char * s2, size_t n)
{
	int c = 0;
	while(*s1 != 0 && *s1 == *s2 && c < n)
	{
		++s1;
		++s2;
		++c;
	}

	return (unsigned char) *s1 - (unsigned char) *s2;
}

