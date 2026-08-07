#ifndef ITT_ELF_LOAD_H
#define ITT_ELF_LOAD_H

#include "ittrium.h"
#include <stddef.h>
#include <stdint.h>

#ifndef ELF_LOAD_BASE
#define ELF_LOAD_BASE  0x40200000u
#endif
#ifndef ELF_LOAD_SIZE
#define ELF_LOAD_SIZE  (512u * 1024u)
#endif

#define ITRM_MAGIC  0x4D525449u /* 'ITRM' LE */

struct itrm_hdr {
  uint32_t magic;
  uint32_t entry_off;
  uint32_t size;
};

struct elf_image {
  void *base;
  size_t size;
  uintptr_t entry;
  int owned; /* free on unload */
};

int elf_load(const char *path, struct elf_image *out);
void elf_unload(struct elf_image *img);

/* Load and create+activate task at entry */
ER elf_run(const char *path, ID tskid, VP stack, SIZE stksz, PRI pri);

#endif
