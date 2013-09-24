// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "utils.h"

size_t strlen(const char * s)
{
	size_t counter = 0;
	for(; counter[s] != 0; ++counter);
	return counter;
}

void strcpy(char * restrict dest, const char * restrict src)
{
	for(unsigned i = 0; i < strlen(src) + 1; ++i)
		dest[i] = src[i];
}

char * strsep(char ** string, char delim)
{
	char * retval = *string;

	if(*string == 0)
		return 0;

	for(unsigned i = 0; (*string)[i] != 0; ++i)
	{
		if((*string)[i] == delim)
		{
			(*string)[i] = 0;
			*string = *string + i;
			break;
		}
	}

	if(retval == *string)
		*string = 0;
	else if(**string == 0)
		*string = 0;
	return retval;
}

bool str_equals(const char * a, const char * b)
{
	for(unsigned i = 0; a[i] != 0 && b[i] != 0; ++i)
		if(a[i] != b[i])
			return false;
	return true;
}

void memclr(void * restrict memory, size_t length)
{
	char * s = memory;
	for(int i = 0; i < length; ++i)
		s[i] = 0;
}

void memcpy(void * dest, void * src, size_t length)
{
	char * s = src, * d = dest;
	for(int i = 0; i < length; ++i)
		d[i] = s[i];
}

