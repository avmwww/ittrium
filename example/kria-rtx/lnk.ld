/*
 * Linker script for Kria / ZynqMP DDR (EL1 no-MMU).
 * Load at 1 MiB — leaves room for FSBL/ATF/U-Boot near 0x0.
 */
ENTRY(_start)

MEMORY
{
	RAM (rwx) : ORIGIN = 0x00100000, LENGTH = 127M
}

SECTIONS
{
	. = 0x00100000;

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
