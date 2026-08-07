#include "romfs.h"
#include <stdlib.h>
#include <string.h>

struct romfs_file {
  const struct romfs_entry *ent;
  size_t pos;
};

static struct romfs g_builtin;
static int g_builtin_inited;

static void ensure_builtin(void)
{
  static const char readme[] = "ittrium romfs\n";
  static const char hello[] = "hello from romfs\n";

  if (g_builtin_inited) return;
  g_builtin.ents = (struct romfs_entry *)malloc(ROMFS_MAX_ENT * sizeof(struct romfs_entry));
  if (g_builtin.ents)
    memset(g_builtin.ents, 0, ROMFS_MAX_ENT * sizeof(struct romfs_entry));
  g_builtin.capacity = ROMFS_MAX_ENT;
  g_builtin.nents = 0;
  g_builtin.owned = 1;
  g_builtin_inited = 1;
  if (!g_builtin.ents) return;

  strncpy(g_builtin.ents[0].name, "readme.txt", ROMFS_NAME_MAX - 1);
  g_builtin.ents[0].data = readme;
  g_builtin.ents[0].size = sizeof(readme) - 1;
  strncpy(g_builtin.ents[1].name, "hello.txt", ROMFS_NAME_MAX - 1);
  g_builtin.ents[1].data = hello;
  g_builtin.ents[1].size = sizeof(hello) - 1;
  g_builtin.nents = 2;
}

static const struct romfs_entry *find_ent(struct romfs *fs, const char *rel)
{
  int i;
  const char *name = rel;
  if (!name) name = "";
  /* strip leading slashes */
  while (*name == '/') name++;
  if (*name == '\0') return 0; /* directory */

  for (i = 0; i < fs->nents; i++) {
    if (strcmp(fs->ents[i].name, name) == 0)
      return &fs->ents[i];
  }
  return 0;
}

static int rom_open(void *fs, const char *rel, int flags, void **out)
{
  struct romfs *r = (struct romfs *)fs;
  const struct romfs_entry *e;
  struct romfs_file *f;

  (void)flags;
  e = find_ent(r, rel);
  if (!e) return -1;

  f = (struct romfs_file *)malloc(sizeof(*f));
  if (!f) return -1;
  f->ent = e;
  f->pos = 0;
  *out = f;
  return 0;
}

static int rom_read(void *file, void *buf, size_t len)
{
  struct romfs_file *f = (struct romfs_file *)file;
  size_t left;

  if (!f || !f->ent) return -1;
  if (f->pos >= f->ent->size) return 0;
  left = f->ent->size - f->pos;
  if (len > left) len = left;
  memcpy(buf, (const char *)f->ent->data + f->pos, len);
  f->pos += len;
  return (int)len;
}

static int rom_write(void *file, const void *buf, size_t len)
{
  (void)file; (void)buf; (void)len;
  return -1;
}

static int rom_close(void *file)
{
  free(file);
  return 0;
}

static int rom_stat(void *fs, const char *rel, struct vfs_stat *st)
{
  struct romfs *r = (struct romfs *)fs;
  const struct romfs_entry *e;
  const char *name = rel ? rel : "";

  while (*name == '/') name++;
  if (*name == '\0') {
    st->type = VFS_TYPE_DIR;
    st->size = 0;
    return 0;
  }
  e = find_ent(r, name);
  if (!e) return -1;
  st->type = VFS_TYPE_REG;
  st->size = e->size;
  return 0;
}

static long rom_lseek(void *file, long off, int whence)
{
  struct romfs_file *f = (struct romfs_file *)file;
  long np;

  if (!f || !f->ent) return -1;
  if (whence == VFS_SEEK_SET) np = off;
  else if (whence == VFS_SEEK_CUR) np = (long)f->pos + off;
  else if (whence == VFS_SEEK_END) np = (long)f->ent->size + off;
  else return -1;
  if (np < 0) return -1;
  if ((size_t)np > f->ent->size) np = (long)f->ent->size;
  f->pos = (size_t)np;
  return np;
}

static int rom_listdir(void *fs, const char *rel, char *name, size_t nlen, int *idx)
{
  struct romfs *r = (struct romfs *)fs;
  const char *p = rel ? rel : "";
  int i;

  while (*p == '/') p++;
  if (*p != '\0') return -1; /* only root dir */
  if (!idx || *idx < 0) return -1;
  i = *idx;
  if (i >= r->nents) return 1;
  if (nlen == 0) return -1;
  strncpy(name, r->ents[i].name, nlen - 1);
  name[nlen - 1] = '\0';
  *idx = i + 1;
  return 0;
}

static const struct vfs_file_ops g_romfs_ops = {
  rom_open, rom_read, rom_write, rom_close, rom_stat, rom_lseek, rom_listdir
};

const struct vfs_file_ops *romfs_ops(void)
{
  return &g_romfs_ops;
}

int romfs_mount(const char *path, struct romfs_entry *entries, int n)
{
  struct romfs *fs;

  if (!entries || n <= 0) {
    ensure_builtin();
    return vfs_mount(path, &g_romfs_ops, &g_builtin);
  }
  fs = (struct romfs *)malloc(sizeof(*fs));
  if (!fs) return -1;
  fs->ents = entries;
  fs->nents = n;
  fs->capacity = n;
  fs->owned = 0;
  return vfs_mount(path, &g_romfs_ops, fs);
}

int romfs_add_builtin(const char *name, const void *data, size_t size)
{
  int i;
  ensure_builtin();
  if (!g_builtin.ents || !name) return -1;
  if (strlen(name) >= ROMFS_NAME_MAX) return -1;

  for (i = 0; i < g_builtin.nents; i++) {
    if (strcmp(g_builtin.ents[i].name, name) == 0) {
      g_builtin.ents[i].data = data;
      g_builtin.ents[i].size = size;
      return 0;
    }
  }
  if (g_builtin.nents >= g_builtin.capacity) return -1;
  strncpy(g_builtin.ents[g_builtin.nents].name, name, ROMFS_NAME_MAX - 1);
  g_builtin.ents[g_builtin.nents].name[ROMFS_NAME_MAX - 1] = '\0';
  g_builtin.ents[g_builtin.nents].data = data;
  g_builtin.ents[g_builtin.nents].size = size;
  g_builtin.nents++;
  return 0;
}
