/*          Mordax simple standalone application linker script         */
/*   (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>   */
/* Report bugs and issues on <http://github.com/skordal/mordax/issues> */

/*
 * This linker script is for very simple test applications that are
 * loaded as the initial process for the kernel. Because of this, all
 * the memory allocated for the process is mapped as executable and
 * read-write. This linker script reflects this by putting every
 * section into the .text section.
 */

SECTIONS
{
	ENTRY(_start)

	.text 0x1000 :
	{
		image_start = .;
		*(.init*)
		*(.text*)
		*(.rodata*)
		*(.data*)
		*(.bss*)
		image_end = .;
		image_size = image_start - image_end;
	}

	/DISCARD/ :
	{
		*(.comment)
		*(.ARM.attributes)
	}
}


