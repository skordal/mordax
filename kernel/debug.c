// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "types.h"

#include <stdarg.h>
#include <stdbool.h>

static struct debug_driver * driver;

// Prints a character to the debug terminal:
static void debug_putc(char c);
// Prints a signed decimal to the debug terminal:
static void debug_putd(int n);
// Prints a hexadecimal number to the debug terminal:
static void debug_puth(uint32_t n);
// Prints a string to the debug terminal:
static void debug_puts(const char * s);

void debug_set_output_driver(struct debug_driver * d)
{
	driver = d;
	// TODO: Add a buffer for storing debug messages
	// TODO: Flush the buffer to the output device when this function is called
}

void debug_printf(const char * format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	for(int i = 0; format[i] != 0; ++i)
	{
		if(format[i] == '%')
		{
			switch(format[i + 1])
			{
				case 'd':
					debug_putd(va_arg(arguments, int));
					break;
				case 's':
					debug_puts(va_arg(arguments, const char *));
					break;
				case 'p':
				case 'x':
					debug_puth(va_arg(arguments, uint32_t));
					break;
				case '%':
					debug_putc('%');
					break;
				case '0':
				default:
					break;
			}

			++i;
		} else {
			debug_putc(format[i]);
		}
	}

	va_end(arguments);
}

static void debug_putc(char c)
{
	// TODO: remove this hard-coded address:
	static volatile char * uart = (volatile char *) 0x49020000;

	if(c == '\n')
		debug_putc('\r');

	if(driver != 0)
		driver->putc(c);
	else {
		while(!(uart[0x14] & (1 << 5)));
		*uart = c;
	}
}

static void debug_putd(int n)
{
	static char buffer[10];
	bool inside_num = false;

	// If the number is negative, print a minus sign and convert it to
	// a positive number:
	if(n & 0x80000000)
	{
		n = ~n + 1;
		debug_putc('-');
	}

	for(int i = 0; i < 10; ++i)
		buffer[i] = '0';

	for(int i = 9; i >= 0; --i)
	{
		buffer[i] = '0' + (n % 10);
		n /= 10;
	}

	for(int i = 0; i < 10; ++i)
	{
		// Do not print leading zeroes:
		if(!inside_num && buffer[i] == '0' && i != 9)
			continue;
		inside_num = true;
		debug_putc(buffer[i]);
	}
}

static void debug_puth(uint32_t val)
{
	const char * hex_digits = "0123456789abcdef";
	debug_puts("0x");
	for(int i = 28; i >= 0; i -= 4)
		debug_putc(hex_digits[val >> i & 0xf]);
}

static void debug_puts(const char * string)
{
	if(string == 0)
		debug_puts("(null)");
	else
		for(int i = 0; string[i] != 0; ++i)
			debug_putc(string[i]);
}



