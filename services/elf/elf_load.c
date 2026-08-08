#include "elf_load.h"
#include "vfs.h"
#include <stdlib.h>
#include <string.h>

extern void *memalign(size_t alignment, size_t size);

#define EI_NIDENT     16
#define ET_EXEC       2
#define ET_DYN        3
#define EM_AARCH64    183
#define PT_LOAD       1
#define PT_DYNAMIC    2
#define DT_NULL       0
#define DT_SYMTAB     6
#define DT_RELA       7
#define DT_RELASZ     8
#define DT_RELAENT    9
#define DT_SYMENT     11
#define R_AARCH64_ABS64    257
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

struct elf64_sym {
  uint32_t st_name;
  uint8_t  st_info;
  uint8_t  st_other;
  uint16_t st_shndx;
  uint64_t st_value;
  uint64_t st_size;
};

#define ELF_R_TYPE(i) ((uint32_t)(i))
#define ELF_R_SYM(i)  ((uint32_t)((i) >> 32))

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

static int apply_relocs(uint8_t *img, uint64_t min_v, size_t span, uintptr_t slide,
                        const struct elf64_rela *rela, size_t n,
                        const struct elf64_sym *symtab, size_t nsym)
{
  size_t i;

  for (i = 0; i < n; i++) {
    uint32_t type = ELF_R_TYPE(rela[i].r_info);
    uint32_t si = ELF_R_SYM(rela[i].r_info);
    uint64_t off = rela[i].r_offset;
    uint64_t *loc;
    uintptr_t img_off;

    if (off < min_v || (off - min_v) + sizeof(uint64_t) > span)
      return -1;
    img_off = (uintptr_t)(off - min_v);
    loc = (uint64_t *)(img + img_off);

    if (type == R_AARCH64_RELATIVE) {
      *loc = (uint64_t)slide + (uint64_t)rela[i].r_addend;
    } else if (type == R_AARCH64_ABS64) {
      uint64_t s;
      if (!symtab || si >= nsym)
        return -1;
      s = symtab[si].st_value;
      *loc = (uint64_t)slide + s + (uint64_t)rela[i].r_addend;
    } else {
      return -1; /* unsupported reloc */
    }
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
  int want_reloc;

  if (fsz < sizeof(*eh)) return -1;
  eh = (struct elf64_ehdr *)file;
  if (eh->e_ident[0] != 0x7f || eh->e_ident[1] != 'E' ||
      eh->e_ident[2] != 'L' || eh->e_ident[3] != 'F')
    return -1;
  if (eh->e_ident[4] != 2 || eh->e_ident[5] != 1) return -1;
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

  want_reloc = (eh->e_type == ET_DYN) || have_dyn;

  /* Fixed ET_EXEC in reserved window (no dynamic reloc) */
  if (!want_reloc && eh->e_type == ET_EXEC) {
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
  }

  /* AArch64 ADRP is page-relative: image base must be 4KiB-aligned. */
  img = (uint8_t *)memalign(4096, span ? span : 16);
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
    uint64_t symtab_va = 0, syment = sizeof(struct elf64_sym);
    struct elf64_sym *symtab = 0;
    size_t nsym = 0;

    for (; dyn->d_tag != DT_NULL; dyn++) {
      if (dyn->d_tag == DT_RELA) rela = dyn->d_val;
      else if (dyn->d_tag == DT_RELASZ) relasz = dyn->d_val;
      else if (dyn->d_tag == DT_RELAENT) relaent = dyn->d_val;
      else if (dyn->d_tag == DT_SYMTAB) symtab_va = dyn->d_val;
      else if (dyn->d_tag == DT_SYMENT) syment = dyn->d_val;
    }
    if (symtab_va && syment == sizeof(struct elf64_sym) &&
        symtab_va >= min_v && (symtab_va - min_v) < span) {
      symtab = (struct elf64_sym *)(img + (size_t)(symtab_va - min_v));
      /* Bound by remaining image; dynsym is usually small */
      nsym = (span - (size_t)(symtab_va - min_v)) / sizeof(struct elf64_sym);
      if (nsym > 256) nsym = 256;
    }
    if (rela && relasz && relaent) {
      struct elf64_rela *r =
        (struct elf64_rela *)(img + (size_t)(rela - min_v));
      size_t n = (size_t)(relasz / relaent);
      if (apply_relocs(img, min_v, span, slide, r, n, symtab, nsym) != 0) {
        free(img);
        return -1;
      }
    }
  } else if (want_reloc) {
    free(img);
    return -1; /* ET_DYN without PT_DYNAMIC */
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

struct elf_mod {
  ID tskid;
  struct elf_image img;
  void *stack;
  int stack_owned;
  int used;
};

static struct elf_mod g_mods[ELF_MODULES_MAX];

static struct elf_mod *mod_find(ID tskid)
{
  int i;
  for (i = 0; i < ELF_MODULES_MAX; i++)
    if (g_mods[i].used && g_mods[i].tskid == tskid)
      return &g_mods[i];
  return 0;
}

static struct elf_mod *mod_alloc(void)
{
  int i;
  for (i = 0; i < ELF_MODULES_MAX; i++)
    if (!g_mods[i].used)
      return &g_mods[i];
  return 0;
}

static void mod_free_slot(struct elf_mod *m)
{
  if (!m) return;
  elf_unload(&m->img);
  if (m->stack_owned && m->stack)
    free(m->stack);
  memset(m, 0, sizeof(*m));
}

ER_ID elf_run(const char *path, ID tskid, VP stack, SIZE stksz, PRI pri)
{
  struct elf_image img;
  struct elf_mod *mod;
  T_CTSK ctsk;
  ER er;
  ER_ID id;
  const char *base = path;
  int stack_owned = 0;

  if (!path || stksz == 0) return E_PAR;
  if (elf_load(path, &img) < 0) return E_PAR;

  mod = mod_alloc();
  if (!mod) {
    elf_unload(&img);
    return E_NOID;
  }

  if (!stack) {
    stack = malloc((size_t)stksz);
    if (!stack) {
      elf_unload(&img);
      return E_NOMEM;
    }
    stack_owned = 1;
  }

  if (path) {
    const char *p;
    for (p = path; *p; p++)
      if (*p == '/') base = p + 1;
  }

  memset(&ctsk, 0, sizeof(ctsk));
  ctsk.tskatr = TA_HLNG | TA_NAME;
  ctsk.exinf = 0;
  ctsk.task = (FP)img.entry;
  ctsk.itskpri = pri;
  ctsk.stksz = stksz;
  ctsk.stk = stack;
  ctsk.name = base;

  if (tskid == 0) {
    id = acre_tsk(&ctsk);
    if (id <= 0) {
      if (stack_owned) free(stack);
      elf_unload(&img);
      return id < 0 ? id : E_NOID;
    }
  } else {
    er = cre_tsk(tskid, &ctsk);
    if (er != E_OK) {
      if (stack_owned) free(stack);
      elf_unload(&img);
      return (ER_ID)er;
    }
    id = (ER_ID)tskid;
  }

  er = act_tsk((ID)id);
  if (er != E_OK) {
    del_tsk((ID)id);
    if (stack_owned) free(stack);
    elf_unload(&img);
    return (ER_ID)er;
  }

  mod->used = 1;
  mod->tskid = (ID)id;
  mod->img = img;
  mod->img.owned = 1;
  mod->stack = stack;
  mod->stack_owned = stack_owned;
  return id;
}

ER elf_kill(ID tskid)
{
  struct elf_mod *mod;
  ER er;

  if (tskid < TMIN_TSKID)
    return E_ID;
  mod = mod_find(tskid);
  if (!mod)
    return E_NOEXS;

  er = del_tsk(tskid);
  if (er != E_OK && er != E_NOEXS)
    return er;

  mod_free_slot(mod);
  return E_OK;
}
