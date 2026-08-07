#include "ramfs.h"
#include <stdlib.h>
#include <string.h>

#define RAMFS_MAX_FILES  16
#define RAMFS_NAME_MAX   48
#define RAMFS_MAX_SIZE   4096

struct ramfs_entry {
  char name[RAMFS_NAME_MAX];
  uint8_t *data;
  size_t size;
  size_t cap;
  int used;
};

struct ramfs {
  struct ramfs_entry ents[RAMFS_MAX_FILES];
};

struct ramfs_file {
  struct ramfs_entry *ent;
  size_t pos;
};

static struct ramfs g_fs;

static struct ramfs_entry *find_ent(struct ramfs *fs, const char *rel)
{
  int i;
  const char *name = rel ? rel : "";
  while (*name == '/') name++;
  if (*name == '\0') return 0;
  for (i = 0; i < RAMFS_MAX_FILES; i++) {
    if (fs->ents[i].used && strcmp(fs->ents[i].name, name) == 0)
      return &fs->ents[i];
  }
  return 0;
}

static struct ramfs_entry *alloc_ent(struct ramfs *fs, const char *name)
{
  int i;
  for (i = 0; i < RAMFS_MAX_FILES; i++) {
    if (!fs->ents[i].used) {
      memset(&fs->ents[i], 0, sizeof(fs->ents[i]));
      strncpy(fs->ents[i].name, name, RAMFS_NAME_MAX - 1);
      fs->ents[i].used = 1;
      return &fs->ents[i];
    }
  }
  return 0;
}

static int rf_open(void *fs, const char *rel, int flags, void **out)
{
  struct ramfs *r = (struct ramfs *)fs;
  struct ramfs_entry *e;
  struct ramfs_file *f;
  const char *name = rel ? rel : "";

  while (*name == '/') name++;
  if (*name == '\0') return -1;

  e = find_ent(r, name);
  if (!e) {
    if (!(flags & VFS_O_CREAT)) return -1;
    e = alloc_ent(r, name);
    if (!e) return -1;
  }
  if ((flags & VFS_O_TRUNC) && e->data) {
    free(e->data);
    e->data = 0;
    e->size = 0;
    e->cap = 0;
  }

  f = (struct ramfs_file *)malloc(sizeof(*f));
  if (!f) return -1;
  f->ent = e;
  f->pos = (flags & VFS_O_APPEND) ? e->size : 0;
  *out = f;
  return 0;
}

static int rf_read(void *file, void *buf, size_t len)
{
  struct ramfs_file *f = (struct ramfs_file *)file;
  size_t left;
  if (!f || !f->ent) return -1;
  if (f->pos >= f->ent->size) return 0;
  left = f->ent->size - f->pos;
  if (len > left) len = left;
  if (len && f->ent->data)
    memcpy(buf, f->ent->data + f->pos, len);
  f->pos += len;
  return (int)len;
}

static int rf_write(void *file, const void *buf, size_t len)
{
  struct ramfs_file *f = (struct ramfs_file *)file;
  size_t need;
  uint8_t *nbuf;

  if (!f || !f->ent) return -1;
  need = f->pos + len;
  if (need > RAMFS_MAX_SIZE) return -1;
  if (need > f->ent->cap) {
    size_t ncap = f->ent->cap ? f->ent->cap * 2 : 256;
    while (ncap < need) ncap *= 2;
    if (ncap > RAMFS_MAX_SIZE) ncap = RAMFS_MAX_SIZE;
    nbuf = (uint8_t *)realloc(f->ent->data, ncap);
    if (!nbuf) return -1;
    f->ent->data = nbuf;
    f->ent->cap = ncap;
  }
  memcpy(f->ent->data + f->pos, buf, len);
  f->pos += len;
  if (f->pos > f->ent->size) f->ent->size = f->pos;
  return (int)len;
}

static int rf_close(void *file)
{
  free(file);
  return 0;
}

static int rf_stat(void *fs, const char *rel, struct vfs_stat *st)
{
  struct ramfs_entry *e;
  const char *name = rel ? rel : "";
  while (*name == '/') name++;
  if (*name == '\0') {
    st->type = VFS_TYPE_DIR;
    st->size = 0;
    return 0;
  }
  e = find_ent((struct ramfs *)fs, name);
  if (!e) return -1;
  st->type = VFS_TYPE_REG;
  st->size = e->size;
  return 0;
}

static long rf_lseek(void *file, long off, int whence)
{
  struct ramfs_file *f = (struct ramfs_file *)file;
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

static int rf_listdir(void *fs, const char *rel, char *name, size_t nlen, int *idx)
{
  struct ramfs *r = (struct ramfs *)fs;
  const char *p = rel ? rel : "";
  int i, seen = 0;
  while (*p == '/') p++;
  if (*p != '\0') return -1;
  if (!idx || *idx < 0) return -1;
  for (i = 0; i < RAMFS_MAX_FILES; i++) {
    if (!r->ents[i].used) continue;
    if (seen == *idx) {
      strncpy(name, r->ents[i].name, nlen - 1);
      name[nlen - 1] = '\0';
      *idx = seen + 1;
      return 0;
    }
    seen++;
  }
  return 1;
}

static const struct vfs_file_ops g_ramfs_ops = {
  rf_open, rf_read, rf_write, rf_close, rf_stat, rf_lseek, rf_listdir
};

int ramfs_mount(const char *path)
{
  memset(&g_fs, 0, sizeof(g_fs));
  return vfs_mount(path, &g_ramfs_ops, &g_fs);
}
