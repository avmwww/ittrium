/*
 * Linker script for ittrium loadable AArch64 apps (position-independent).
 *
 * Preferred build (relocated by services/elf/elf_load.c):
 *   aarch64-none-elf-gcc -fPIE -ffreestanding -nostdlib \
 *     -Wl,-T$(ITTRIUM)/services/elf/app.ld \
 *     -Wl,-pie -Wl,-Bsymbolic --no-dynamic-linker \
 *     -o app.elf start.S main.c
 *
 * Link at VA 0; the loader slides the image and applies R_AARCH64_RELATIVE
 * (and R_AARCH64_ABS64 when present). Use -Bsymbolic so internal refs become
 * RELATIVE instead of unresolved ABS64 against .dynsym.
 *
 * Alternate: ET_EXEC linked into ELF_LOAD_BASE..+ELF_LOAD_SIZE (no reloc).
 * See elf_load.h for the reserved window — prefer PIE + reloc instead.
 */
OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(_start)

PHDRS
{
  text PT_LOAD FLAGS(5); /* R+X */
  data PT_LOAD FLAGS(6); /* R+W */
  dyn  PT_DYNAMIC FLAGS(4);
}

SECTIONS
{
  . = 0;

  .text : {
    KEEP(*(.text.boot))
    *(.text .text.*)
  } :text

  .rodata : {
    *(.rodata .rodata.*)
  } :text

  . = ALIGN(16);
  .data : {
    *(.data .data.*)
    *(.sdata .sdata.*)
  } :data

  .got : {
    *(.got .got.*)
  } :data

  . = ALIGN(16);
  .bss : {
    __bss_start = .;
    *(.sbss .sbss.*)
    *(.bss .bss.*)
    *(COMMON)
    . = ALIGN(16);
    __bss_end = .;
  } :data

  /* Dynamic tables must stay in a PT_LOAD so the loader can relocate in place */
  .dynamic : {
    *(.dynamic)
  } :data :dyn

  .dynsym : { *(.dynsym) } :data
  .dynstr : { *(.dynstr) } :data
  .hash : { *(.hash) } :data
  .gnu.hash : { *(.gnu.hash) } :data
  .rela.dyn : { *(.rela.dyn) *(.rela*) } :data

  /DISCARD/ : {
    *(.comment)
    *(.note*)
    *(.eh_frame*)
    *(.ARM.attributes)
  }
}
