/*             Mordax initial process loader linker script             */
/*   (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>   */
/* Report bugs and issues on <http://github.com/skordal/mordax/issues> */

SECTIONS
{
	ENTRY(loader)

	.text 0x1000 :
	{
		armv7-loader.o(*)
		*(.text*)
		*(.rodata*)
		*(.data*)
		*(.bss*)
	}

	archive_start = .;

	/DISCARD/ :
	{
		*(.comment)
		*(.ARM.attributes)
	}
}

