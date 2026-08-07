/*
 * Linker script for QEMU virt (-kernel loads at 0x40000000)
 */
ENTRY(_start)

MEMORY
{
	RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 128M
}

SECTIONS
{
	. = 0x40000000;

	.text : {
		KEEP(*(.text.boot))
		KEEP(*(.text.vectors))
		*(.text .text.*)
	} > RAM

	.rodata : {
		*(.rodata .rodata.*)
	} > RAM

	.data : {
		__data_start = .;
		*(.data .data.*)
		__data_end = .;
	} > RAM

	.bss : {
		. = ALIGN(16);
		__bss_start = .;
		*(.bss .bss.*)
		*(COMMON)
		. = ALIGN(16);
		__bss_end = .;
	} > RAM

	. = ALIGN(16);
	__stack_bottom = .;
	. = . + 0x4000;
	__stack_top = .;

	_end = .;
	end = .;
}
