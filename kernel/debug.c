// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "api/types.h"

#include <stdarg.h>
#include <stdbool.h>

static struct debug_driver * driver;

#ifdef CONFIG_EARLY_DEBUG
// Prints a character to the debug terminal, used before a driver has been set
// by debug_set_output_driver:
extern void early_putc(char c);
#endif

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
				case 'c':
					debug_putc(va_arg(arguments, int));
					break;
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
	if(c == '\n')
		debug_putc('\r');

	if(driver != 0)
		driver->putc(c);
	else {
#ifdef CONFIG_EARLY_DEBUG
		early_putc(c);
#endif
	}
}

static void debug_putd(int n)
{
	bool leading = true;

	// If the input number is 0, just print 0 and return:
	if(n == 0)
	{
		debug_putc('0');
		return;
	}

	// If the number is negative, print a minus sign and convert it to
	// a positive number:
	if(n & 0x80000000)
	{
		n = ~n + 1;
		debug_putc('-');
	}

	for(unsigned int i = 1000000000; i > 0; i /= 10)
	{
		if(n / i == 0 && !leading)
			debug_putc('0');
		else if(n / i != 0)
		{
			debug_putc('0' + (n / i));
			n %= i;
			leading = false;
		}
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



