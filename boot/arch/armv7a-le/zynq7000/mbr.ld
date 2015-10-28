ENTRY(bootloader)

MEMORY
{
	MBR (rwx) :
		ORIGIN = 1M,
		LENGTH = 446
}

SECTIONS
{
	.text : {
		*(.text .rodata)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
	}
}
