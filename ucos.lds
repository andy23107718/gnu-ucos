OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")

ENTRY(_start)

SECTIONS 
{
	. = 0x30000000;

	. = ALIGN(4);
	.text      :
	{
		init/2440init.o		(.text)
		init/cpnand.o 		(.text)
		*(.text)
	}

	. = ALIGN(4);
	.rodata : { *(.rodata) }

	. = ALIGN(4);
	.data : { *(.data) }

	.bss ALIGN(4)  : { *(.bss)  *(COMMON) }
}
