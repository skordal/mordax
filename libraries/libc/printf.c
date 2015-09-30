// The Mordax Standard Library
// (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <mordax.h>

// Converts a group of four bits into a hexadecimal value;
inline static char hexdigit(long value, int digit, bool uppercase)
{
	const char * lowercase_digits = "0123456789abcdef";
	const char * uppercase_digits = "0123456789ABCDEF";

	if(uppercase)
		return uppercase_digits[value >> (digit << 2) & 0xf];
	else
		return lowercase_digits[value >> (digit << 2) & 0xf];
}

// Integer-to-hex function:
static void itoh(char * b, unsigned v);
// Integer-to-string function:
static void itoa(char * b, unsigned n);

int mprintf(char * restrict format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	char * buffer = malloc(MPRINTF_MAXLEN);
	int retval = vsnprintf(buffer, MPRINTF_MAXLEN, format, arguments);
	mordax_system(MORDAX_SYSTEM_DEBUG, buffer);

	free(buffer);
	va_end(arguments);

	return retval;
}

int vsnprintf(char * restrict s, size_t size, const char * restrict format, va_list arguments)
{
	int out = 0;
	for(int i = 0; format[i] != 0 && out < size - 1; ++i)
	{
		if(format[i] == '%')
		{
			switch(format[i + 1])
			{
				case 'c':
				{
					char val = va_arg(arguments, int);
					s[out++] = val;	
					break;
				}
				case 'd':
				{
					int val = va_arg(arguments, int);

					if(val < 0)
					{
						s[out++] = '-';
						val = ~val + 1;
					}

					char buffer[11];
					itoa(buffer, val);

					bool inside_num = false;
					for(int i = 0; i < 10 && out < size - 1; ++i)
					{
						// Do not print leading zeroes:
						if(!inside_num && buffer[i] == '0' && i != 9)
							continue;
						inside_num = true;
						s[out++] = buffer[i];
					}
					break;
				}
				case 's':
				{
					const char * val = va_arg(arguments, const char *);
					for(int i = 0; val[i] != 0 && out < size - 1; ++i)
						s[out++] = val[i];
					break;
				}
				case 'x':
				{
					unsigned int val = va_arg(arguments, unsigned int);
					for(int i = 7; i >= 0 && out < size - 1; --i)
						s[out++] = hexdigit(val, i, false);
					break;
				}
				case 'X':
				{
					unsigned int val = va_arg(arguments, unsigned int);
					for(int i = 7; i >= 0 && out < size - 1; --i)
						s[out++] = hexdigit(val, i, true);
					break;
				}
				default:
					break;
			}

			++i;
		} else
			s[out++] = format[i];
	}

	s[out] = 0;
	return out;
}

int snprintf(char * restrict s, size_t size, const char * restrict format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	int retval = vsnprintf(s, size, format, arguments);

	va_end(arguments);
	return retval;
}

static void itoa(char * b, unsigned n)
{
	for(int i = 0; i < 10; ++i)
		b[i] = '0';

	for(int i = 9; i >= 0; --i)
	{
		b[i] = '0' + (n % 10);
		n /= 10;
	}
}

static void itoh(char * b, unsigned v)
{
	for(int i = 7, out = 0; i >= 0; --i)
		b[out++] = hexdigit(v, i, false);
}

