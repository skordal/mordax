/*                   Mordax simple_test linker script                  */
/*   (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>   */
/* Report bugs and issues on <http://github.com/skordal/mordax/issues> */

SECTIONS
{
	.text 0x1000 :
	{
		*(.text)
	}

	.data ALIGN(4096) :
	{
		*(.data)
	}

	.bss ALIGN(4) :
	{
		*(.bss)
	}
}

