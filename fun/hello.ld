ENTRY(sayhello)

SECTIONS
{
	. = 8M;
        .text . : {
                *(.text .rodata)
        }
        .data . : {
                *(.data)
        }
        .bss . : {
                *(.bss)
        }
}
