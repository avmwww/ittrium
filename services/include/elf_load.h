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

#ifndef ELF_MODULES_MAX
#if TRSV_TSKID > 0
#define ELF_MODULES_MAX  TRSV_TSKID
#else
#define ELF_MODULES_MAX  4
#endif
#endif

/*
 * Preferred apps: PIE linked with services/elf/app.ld (-fPIE -pie -Bsymbolic).
 * Loader slides the image and applies R_AARCH64_RELATIVE / R_AARCH64_ABS64.
 *
 * Legacy: ET_EXEC whose PT_LOAD fit [ELF_LOAD_BASE, +ELF_LOAD_SIZE) — no reloc.
 */

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

/* Load and create+activate task at entry.
 * tskid == 0 → acre_tsk (needs TRSV_TSKID); else cre_tsk(tskid).
 * stack == 0 → malloc(stksz), freed by elf_kill.
 * Returns assigned task ID (>= TMIN_TSKID) or negative ER. */
ER_ID elf_run(const char *path, ID tskid, VP stack, SIZE stksz, PRI pri);

/* Stop task, free image (+ owned stack). */
ER elf_kill(ID tskid);

#endif
