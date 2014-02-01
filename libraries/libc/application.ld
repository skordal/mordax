/*                   Mordax application linker script                  */
/*   (c) Kristian Klomsten Skordal 2014 <kristian.skordal@gmail.com>   */
/* Report bugs and issues on <http://github.com/skordal/mordax/issues> */

SECTIONS
{
	ENTRY(_start)

	.text 0x1000 :
	{
		__text_start = .;
		*(.init*)
		*(.text*)
	}

	.rodata : ALIGN(4096)
	{
		__rodata_start = .;
		*(.rodata*)
	}

	.data : ALIGN(4096)
	{
		__data_start = .;
		*(.data*)
	}

	.bss : ALIGN(4)
	{
		__bss_start = .;
		*(.bss*)
	}

	__application_end = .;

	/DISCARD/ :
	{
		*(.comment)
		*(.ARM.attributes)
	}
}




