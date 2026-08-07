#include "sysfs.h"
#include "telemetry.h"
#include <stdlib.h>
#include <string.h>

struct sys_file {
  const char *data;
  size_t len;
  size_t pos;
};

static char el_buf[32];
static char frq_buf[32];
static char midr_buf[40];
static char mmu_buf[16];
static char load_buf[64];
static const char loopback_txt[] = "up mtu 65536\n";

static void u64_to_dec(unsigned long long v, char *out, size_t cap)
{
  char tmp[24];
  int i = 0, n = 0;
  if (cap == 0) return;
  if (v == 0) {
    out[0] = '0';
    out[1] = '\0';
    return;
  }
  while (v && i < (int)sizeof(tmp)) {
    tmp[i++] = (char)('0' + (unsigned)(v % 10ull));
    v /= 10ull;
  }
  while (i-- && (size_t)n + 1 < cap)
    out[n++] = tmp[i];
  out[n] = '\0';
}

static void u64_to_hex(unsigned long long v, char *out, size_t cap)
{
  static const char *dig = "0123456789abcdef";
  int i, n = 0;
  if (cap < 3) return;
  out[n++] = '0';
  out[n++] = 'x';
  for (i = 60; i >= 0 && (size_t)n + 1 < cap; i -= 4)
    out[n++] = dig[(v >> i) & 0xfull];
  out[n] = '\0';
}

static unsigned long long read_current_el(void)
{
  unsigned long long v;
  __asm__ volatile("mrs %0, CurrentEL" : "=r"(v));
  return (v >> 2) & 3ull;
}

static unsigned long long read_cntfrq(void)
{
  unsigned long long v;
  __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(v));
  return v;
}

static unsigned long long read_midr(void)
{
  unsigned long long v;
  __asm__ volatile("mrs %0, midr_el1" : "=r"(v));
  return v;
}

static unsigned long long read_sctlr_m(void)
{
  unsigned long long v;
  __asm__ volatile("mrs %0, sctlr_el1" : "=r"(v));
  return v & 1ull;
}

static const char *const sys_files[] = {
  "cpu/el", "cpu/cntfrq", "cpu/midr", "cpu/mmu", "cpu/load",
  "net/loopback", 0
};

static int fill_file(const char *name, const char **data, size_t *len)
{
  if (strcmp(name, "cpu/el") == 0) {
    u64_to_dec(read_current_el(), el_buf, sizeof(el_buf) - 2);
    strcat(el_buf, "\n");
    *data = el_buf;
    *len = strlen(el_buf);
    return 0;
  }
  if (strcmp(name, "cpu/cntfrq") == 0) {
    u64_to_dec(read_cntfrq(), frq_buf, sizeof(frq_buf) - 2);
    strcat(frq_buf, "\n");
    *data = frq_buf;
    *len = strlen(frq_buf);
    return 0;
  }
  if (strcmp(name, "cpu/midr") == 0) {
    u64_to_hex(read_midr(), midr_buf, sizeof(midr_buf) - 2);
    strcat(midr_buf, "\n");
    *data = midr_buf;
    *len = strlen(midr_buf);
    return 0;
  }
  if (strcmp(name, "cpu/mmu") == 0) {
    u64_to_dec(read_sctlr_m(), mmu_buf, sizeof(mmu_buf) - 2);
    strcat(mmu_buf, "\n");
    *data = mmu_buf;
    *len = strlen(mmu_buf);
    return 0;
  }
  if (strcmp(name, "cpu/load") == 0) {
    unsigned idle = idle_cpu_pct();
    unsigned wall = (unsigned)telemetry_wall_ticks;
    char idles[16], walls[16];
    char *p = load_buf;
    const char *s;
    u64_to_dec(idle, idles, sizeof(idles));
    u64_to_dec(wall, walls, sizeof(walls));
    for (s = idles; *s; s++) *p++ = *s;
    for (s = "% idle wall="; *s; s++) *p++ = *s;
    for (s = walls; *s; s++) *p++ = *s;
    *p++ = '\n';
    *p = '\0';
    *data = load_buf;
    *len = strlen(load_buf);
    return 0;
  }
  if (strcmp(name, "net/loopback") == 0) {
    *data = loopback_txt;
    *len = sizeof(loopback_txt) - 1;
    return 0;
  }
  return -1;
}

static int sf_open(void *fs, const char *rel, int flags, void **out)
{
  struct sys_file *h;
  const char *data;
  size_t len;
  const char *name = rel ? rel : "";

  (void)fs;
  (void)flags;
  while (*name == '/')
    name++;
  if (*name == '\0')
    return -1;
  if (fill_file(name, &data, &len) < 0)
    return -1;

  h = (struct sys_file *)malloc(sizeof(*h));
  if (!h)
    return -1;
  h->data = data;
  h->len = len;
  h->pos = 0;
  *out = h;
  return 0;
}

static int sf_read(void *file, void *buf, size_t len)
{
  struct sys_file *f = (struct sys_file *)file;
  size_t left;
  if (!f) return -1;
  if (f->pos >= f->len) return 0;
  left = f->len - f->pos;
  if (len > left) len = left;
  memcpy(buf, f->data + f->pos, len);
  f->pos += len;
  return (int)len;
}

static int sf_write(void *file, const void *buf, size_t len)
{
  (void)file;
  (void)buf;
  (void)len;
  return -1;
}

static int sf_close(void *file)
{
  free(file);
  return 0;
}

static int sf_stat(void *fs, const char *rel, struct vfs_stat *st)
{
  const char *name = rel ? rel : "";
  int i;
  (void)fs;
  while (*name == '/')
    name++;
  if (*name == '\0') {
    st->type = VFS_TYPE_DIR;
    st->size = 0;
    return 0;
  }
  for (i = 0; sys_files[i]; i++) {
    if (strcmp(sys_files[i], name) == 0) {
      st->type = VFS_TYPE_REG;
      st->size = 0;
      return 0;
    }
  }
  if (strcmp(name, "cpu") == 0 || strcmp(name, "net") == 0) {
    st->type = VFS_TYPE_DIR;
    st->size = 0;
    return 0;
  }
  return -1;
}

static long sf_lseek(void *file, long off, int whence)
{
  struct sys_file *f = (struct sys_file *)file;
  long np;
  if (!f) return -1;
  if (whence == VFS_SEEK_SET) np = off;
  else if (whence == VFS_SEEK_CUR) np = (long)f->pos + off;
  else if (whence == VFS_SEEK_END) np = (long)f->len + off;
  else return -1;
  if (np < 0) return -1;
  if ((size_t)np > f->len) np = (long)f->len;
  f->pos = (size_t)np;
  return np;
}

static int sf_listdir(void *fs, const char *rel, char *name, size_t nlen, int *idx)
{
  const char *p = rel ? rel : "";
  static const char *root[] = { "cpu", "net", 0 };
  static const char *cpu[] = { "el", "cntfrq", "midr", "mmu", "load", 0 };
  static const char *net[] = { "loopback", 0 };
  const char *const *list = root;
  int i;
  (void)fs;
  while (*p == '/')
    p++;
  if (strcmp(p, "cpu") == 0)
    list = cpu;
  else if (strcmp(p, "net") == 0)
    list = net;
  else if (*p != '\0')
    return -1;

  if (!idx || *idx < 0)
    return -1;
  i = *idx;
  if (!list[i])
    return 1;
  strncpy(name, list[i], nlen - 1);
  name[nlen - 1] = '\0';
  *idx = i + 1;
  return 0;
}

static const struct vfs_file_ops g_sys_ops = {
  sf_open, sf_read, sf_write, sf_close, sf_stat, sf_lseek, sf_listdir
};

int sysfs_mount(const char *path)
{
  return vfs_mount(path, &g_sys_ops, (void *)1);
}

const struct vfs_file_ops *sysfs_ops(void)
{
  return &g_sys_ops;
}
