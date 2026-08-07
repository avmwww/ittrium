#include "elf_load.h"
#include "vfs.h"
#include <stdlib.h>
#include <string.h>

extern void *memalign(size_t alignment, size_t size);

/* ELF64 little-endian constants */
#define EI_NIDENT     16
#define ET_NONE       0
#define ET_EXEC       2
#define ET_DYN        3
#define EM_AARCH64    183
#define PT_LOAD       1
#define PT_DYNAMIC    2
#define DT_NULL       0
#define DT_RELA       7
#define DT_RELASZ     8
#define DT_RELAENT    9
#define R_AARCH64_RELATIVE 1027

struct elf64_ehdr {
  uint8_t  e_ident[EI_NIDENT];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
};

struct elf64_phdr {
  uint32_t p_type;
  uint32_t p_flags;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
};

struct elf64_dyn {
  int64_t  d_tag;
  uint64_t d_val;
};

struct elf64_rela {
  uint64_t r_offset;
  uint64_t r_info;
  int64_t  r_addend;
};

#define ELF_R_TYPE(i) ((uint32_t)(i))

static int read_all(int fd, void *buf, size_t n)
{
  size_t got = 0;
  while (got < n) {
    int r = vfs_read(fd, (char *)buf + got, n - got);
    if (r <= 0) return -1;
    got += (size_t)r;
  }
  return 0;
}

static int load_file(const char *path, uint8_t **out, size_t *outsz)
{
  struct vfs_stat st;
  int fd;
  uint8_t *buf;

  if (vfs_stat(path, &st) < 0 || st.type != VFS_TYPE_REG) return -1;
  fd = vfs_open(path, VFS_O_RDONLY);
  if (fd < 0) return -1;
  buf = (uint8_t *)malloc(st.size ? st.size : 1);
  if (!buf) { vfs_close(fd); return -1; }
  if (st.size && read_all(fd, buf, st.size) < 0) {
    free(buf); vfs_close(fd); return -1;
  }
  vfs_close(fd);
  *out = buf;
  *outsz = st.size;
  return 0;
}

static int load_itrm(uint8_t *file, size_t fsz, struct elf_image *out)
{
  struct itrm_hdr *h = (struct itrm_hdr *)file;
  uint8_t *img;
  size_t payload;

  if (fsz < sizeof(*h) || h->magic != ITRM_MAGIC) return -1;
  payload = h->size;
  if (sizeof(*h) + payload > fsz) return -1;
  if (h->entry_off >= payload) return -1;

  img = (uint8_t *)malloc(payload);
  if (!img) return -1;
  memcpy(img, file + sizeof(*h), payload);
  out->base = img;
  out->size = payload;
  out->entry = (uintptr_t)(img + h->entry_off);
  out->owned = 1;
  return 0;
}

static int in_exec_window(uint64_t vaddr, uint64_t memsz)
{
  uint64_t base = ELF_LOAD_BASE;
  uint64_t end = base + ELF_LOAD_SIZE;
  if (vaddr < base) return 0;
  if (vaddr + memsz > end) return 0;
  return 1;
}

static int apply_relative(uintptr_t load_bias, uintptr_t slide,
                          const struct elf64_rela *rela, size_t n)
{
  size_t i;
  for (i = 0; i < n; i++) {
    uint32_t type = ELF_R_TYPE(rela[i].r_info);
    uint64_t *loc;
    if (type != R_AARCH64_RELATIVE) continue;
    /* r_offset is vaddr; load_bias maps vaddr -> runtime address */
    loc = (uint64_t *)(load_bias + (uintptr_t)rela[i].r_offset);
    *loc = (uint64_t)slide + (uint64_t)rela[i].r_addend;
  }
  return 0;
}

static int load_elf(uint8_t *file, size_t fsz, struct elf_image *out)
{
  struct elf64_ehdr *eh;
  struct elf64_phdr *ph;
  uint64_t min_v = ~(uint64_t)0, max_v = 0;
  uint16_t i;
  uint8_t *img;
  size_t span;
  uintptr_t slide;
  uint64_t dyn_vaddr = 0;
  int have_dyn = 0;

  if (fsz < sizeof(*eh)) return -1;
  eh = (struct elf64_ehdr *)file;
  if (eh->e_ident[0] != 0x7f || eh->e_ident[1] != 'E' ||
      eh->e_ident[2] != 'L' || eh->e_ident[3] != 'F')
    return -1;
  if (eh->e_ident[4] != 2 || eh->e_ident[5] != 1) return -1; /* ELF64 LE */
  if (eh->e_machine != EM_AARCH64) return -1;
  if (eh->e_type != ET_EXEC && eh->e_type != ET_DYN) return -1;
  if (eh->e_phoff + (uint64_t)eh->e_phnum * eh->e_phentsize > fsz) return -1;

  ph = (struct elf64_phdr *)(file + eh->e_phoff);

  for (i = 0; i < eh->e_phnum; i++) {
    if (ph[i].p_type == PT_DYNAMIC) {
      dyn_vaddr = ph[i].p_vaddr;
      have_dyn = 1;
    }
    if (ph[i].p_type != PT_LOAD) continue;
    if (ph[i].p_vaddr < min_v) min_v = ph[i].p_vaddr;
    if (ph[i].p_vaddr + ph[i].p_memsz > max_v)
      max_v = ph[i].p_vaddr + ph[i].p_memsz;
  }
  if (min_v > max_v) return -1;
  span = (size_t)(max_v - min_v);

  if (eh->e_type == ET_EXEC) {
    /* Absolute load into reserved window if all PT_LOAD fit */
    int ok = 1;
    for (i = 0; i < eh->e_phnum; i++) {
      if (ph[i].p_type != PT_LOAD) continue;
      if (!in_exec_window(ph[i].p_vaddr, ph[i].p_memsz)) ok = 0;
    }
    if (ok) {
      for (i = 0; i < eh->e_phnum; i++) {
        uint8_t *dst;
        if (ph[i].p_type != PT_LOAD) continue;
        dst = (uint8_t *)(uintptr_t)ph[i].p_vaddr;
        memset(dst, 0, (size_t)ph[i].p_memsz);
        if (ph[i].p_filesz) {
          if (ph[i].p_offset + ph[i].p_filesz > fsz) return -1;
          memcpy(dst, file + ph[i].p_offset, (size_t)ph[i].p_filesz);
        }
      }
      out->base = (void *)(uintptr_t)min_v;
      out->size = span;
      out->entry = (uintptr_t)eh->e_entry;
      out->owned = 0;
      return 0;
    }
    /* fall through: treat like DYN (slide into malloc buffer) */
  }

  img = (uint8_t *)memalign(16, span ? span : 16);
  if (!img) return -1;
  memset(img, 0, span);
  slide = (uintptr_t)img - (uintptr_t)min_v;

  for (i = 0; i < eh->e_phnum; i++) {
    uint8_t *dst;
    if (ph[i].p_type != PT_LOAD) continue;
    dst = img + (size_t)(ph[i].p_vaddr - min_v);
    if (ph[i].p_filesz) {
      if (ph[i].p_offset + ph[i].p_filesz > fsz) { free(img); return -1; }
      memcpy(dst, file + ph[i].p_offset, (size_t)ph[i].p_filesz);
    }
  }

  if (have_dyn) {
    struct elf64_dyn *dyn =
      (struct elf64_dyn *)(img + (size_t)(dyn_vaddr - min_v));
    uint64_t rela = 0, relasz = 0, relaent = sizeof(struct elf64_rela);
    for (; dyn->d_tag != DT_NULL; dyn++) {
      if (dyn->d_tag == DT_RELA) rela = dyn->d_val;
      else if (dyn->d_tag == DT_RELASZ) relasz = dyn->d_val;
      else if (dyn->d_tag == DT_RELAENT) relaent = dyn->d_val;
    }
    if (rela && relasz && relaent) {
      struct elf64_rela *r =
        (struct elf64_rela *)(img + (size_t)(rela - min_v));
      size_t n = (size_t)(relasz / relaent);
      apply_relative((uintptr_t)img - (uintptr_t)min_v, slide, r, n);
    }
  }

  out->base = img;
  out->size = span;
  out->entry = (uintptr_t)eh->e_entry + slide;
  out->owned = 1;
  return 0;
}

int elf_load(const char *path, struct elf_image *out)
{
  uint8_t *file = 0;
  size_t fsz = 0;
  int rc;

  if (!path || !out) return -1;
  memset(out, 0, sizeof(*out));
  if (load_file(path, &file, &fsz) < 0) return -1;

  if (fsz >= sizeof(struct itrm_hdr) &&
      ((struct itrm_hdr *)file)->magic == ITRM_MAGIC)
    rc = load_itrm(file, fsz, out);
  else
    rc = load_elf(file, fsz, out);

  free(file);
  return rc;
}

void elf_unload(struct elf_image *img)
{
  if (!img) return;
  if (img->owned && img->base) free(img->base);
  memset(img, 0, sizeof(*img));
}

ER elf_run(const char *path, ID tskid, VP stack, SIZE stksz, PRI pri)
{
  struct elf_image img;
  T_CTSK ctsk;
  ER er;

  if (elf_load(path, &img) < 0) return E_PAR;

  memset(&ctsk, 0, sizeof(ctsk));
  ctsk.tskatr = TA_HLNG;
  ctsk.exinf = 0;
  ctsk.task = (FP)img.entry;
  ctsk.itskpri = pri;
  ctsk.stksz = stksz;
  ctsk.stk = stack;

  er = cre_tsk(tskid, &ctsk);
  if (er != E_OK) {
    elf_unload(&img);
    return er;
  }
  /* image stays resident; owned buffer not freed */
  img.owned = 0;
  er = act_tsk(tskid);
  return er;
}
