/*           The Mordax Microkernel Linker Script for ARMv7            */
/*      (c) Kristian Klomsten Skordal <kristian.skordal@gmail.com>     */
/* Report bugs and issues on <http://github.com/skordal/mordax/issues> */

SECTIONS
{
	load_address = 0x81000000;
	.init load_address :
	{
		*(.init*)
	}

	kernel_address = 0xc0000000;
	. = kernel_address + SIZEOF(.init);
	text_start = .;
	.text : AT(LOADADDR(.init) + SIZEOF(.init))
	{
		*(.text*)
	}

	.data ALIGN(4096) :
	{
		data_start = .;
		*(.rodata*)
		*(.data*)
	}

	.bss ALIGN(4) :
	{
		bss_start = .;
		*(.bss*)
	}
	bss_end = ALIGN(4);

	/DISCARD/ :
	{
		*(.comment)
		*(.ARM.attributes)
	}
}

