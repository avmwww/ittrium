#include "lfs_port.h"
#include "lfs.h"
#include <string.h>
#include <stdlib.h>

#define BD_SIZE (LFS_PORT_BLOCK_SIZE * LFS_PORT_BLOCK_COUNT)

static uint8_t bd_mem[BD_SIZE];
static lfs_t lfs;
static struct lfs_config cfg;
static int mounted;
static char mount_path_saved[VFS_PATH_MAX];
static uint8_t read_buf[LFS_PORT_BLOCK_SIZE];
static uint8_t prog_buf[LFS_PORT_BLOCK_SIZE];
static uint8_t look_buf[16];

static int bd_read(const struct lfs_config *c, lfs_block_t block,
                   lfs_off_t off, void *buffer, lfs_size_t size)
{
  (void)c;
  if (block >= LFS_PORT_BLOCK_COUNT)
    return LFS_ERR_IO;
  memcpy(buffer, bd_mem + block * LFS_PORT_BLOCK_SIZE + off, size);
  return 0;
}

static int bd_prog(const struct lfs_config *c, lfs_block_t block,
                   lfs_off_t off, const void *buffer, lfs_size_t size)
{
  (void)c;
  if (block >= LFS_PORT_BLOCK_COUNT)
    return LFS_ERR_IO;
  memcpy(bd_mem + block * LFS_PORT_BLOCK_SIZE + off, buffer, size);
  return 0;
}

static int bd_erase(const struct lfs_config *c, lfs_block_t block)
{
  (void)c;
  if (block >= LFS_PORT_BLOCK_COUNT)
    return LFS_ERR_IO;
  memset(bd_mem + block * LFS_PORT_BLOCK_SIZE, 0xff, LFS_PORT_BLOCK_SIZE);
  return 0;
}

static int bd_sync(const struct lfs_config *c)
{
  (void)c;
  return 0;
}

enum { FH_FILE = 1, FH_DIR = 2 };

struct lfs_fh {
  int kind;
  union {
    lfs_file_t file;
    struct {
      lfs_dir_t dir;
      int listing; /* opened as dir for readdir via file? unused */
    } d;
  } u;
};

static int lfs_flags_from_vfs(int flags)
{
  int f = 0;
  if ((flags & VFS_O_RDWR) == VFS_O_RDWR) f |= LFS_O_RDWR;
  else if (flags & VFS_O_WRONLY) f |= LFS_O_WRONLY;
  else f |= LFS_O_RDONLY;
#ifndef LFS_READONLY
  if (flags & VFS_O_CREAT) f |= LFS_O_CREAT;
  if (flags & VFS_O_TRUNC) f |= LFS_O_TRUNC;
  if (flags & VFS_O_APPEND) f |= LFS_O_APPEND;
#endif
  return f;
}

static int lp_open(void *fs, const char *rel, int flags, void **out)
{
  struct lfs_fh *fh;
  int rc;
  const char *path = rel;
  struct lfs_info info;

  (void)fs;
  if (!path || path[0] == '\0') path = "/";
  /* try as file first */
  fh = (struct lfs_fh *)malloc(sizeof(*fh));
  if (!fh) return -1;
  memset(fh, 0, sizeof(*fh));

  rc = lfs_stat(&lfs, path, &info);
  if (rc == 0 && info.type == LFS_TYPE_DIR) {
    fh->kind = FH_DIR;
    rc = lfs_dir_open(&lfs, &fh->u.d.dir, path);
    if (rc < 0) { free(fh); return rc; }
    *out = fh;
    return 0;
  }

  fh->kind = FH_FILE;
  rc = lfs_file_open(&lfs, &fh->u.file, path, lfs_flags_from_vfs(flags));
  if (rc < 0) { free(fh); return rc; }
  *out = fh;
  return 0;
}

static int lp_read(void *file, void *buf, size_t len)
{
  struct lfs_fh *fh = (struct lfs_fh *)file;
  lfs_ssize_t n;
  if (!fh || fh->kind != FH_FILE) return -1;
  n = lfs_file_read(&lfs, &fh->u.file, buf, (lfs_size_t)len);
  return (int)n;
}

static int lp_write(void *file, const void *buf, size_t len)
{
  struct lfs_fh *fh = (struct lfs_fh *)file;
  lfs_ssize_t n;
  if (!fh || fh->kind != FH_FILE) return -1;
  n = lfs_file_write(&lfs, &fh->u.file, buf, (lfs_size_t)len);
  return (int)n;
}

static int lp_close(void *file)
{
  struct lfs_fh *fh = (struct lfs_fh *)file;
  int rc = 0;
  if (!fh) return -1;
  if (fh->kind == FH_FILE)
    rc = lfs_file_close(&lfs, &fh->u.file);
  else if (fh->kind == FH_DIR)
    rc = lfs_dir_close(&lfs, &fh->u.d.dir);
  free(fh);
  return rc;
}

static int lp_stat(void *fs, const char *rel, struct vfs_stat *st)
{
  struct lfs_info info;
  int rc;
  const char *path = rel;
  (void)fs;
  if (!path || path[0] == '\0') path = "/";
  rc = lfs_stat(&lfs, path, &info);
  if (rc < 0) return rc;
  st->type = (info.type == LFS_TYPE_DIR) ? VFS_TYPE_DIR : VFS_TYPE_REG;
  st->size = info.size;
  return 0;
}

static long lp_lseek(void *file, long off, int whence)
{
  struct lfs_fh *fh = (struct lfs_fh *)file;
  int w;
  if (!fh || fh->kind != FH_FILE) return -1;
  if (whence == VFS_SEEK_SET) w = LFS_SEEK_SET;
  else if (whence == VFS_SEEK_CUR) w = LFS_SEEK_CUR;
  else if (whence == VFS_SEEK_END) w = LFS_SEEK_END;
  else return -1;
  return (long)lfs_file_seek(&lfs, &fh->u.file, (lfs_soff_t)off, w);
}

static int lp_listdir(void *fs, const char *rel, char *name, size_t nlen, int *idx)
{
  lfs_dir_t dir;
  struct lfs_info info;
  int rc, skip, i;
  const char *path = rel;

  (void)fs;
  if (!path || path[0] == '\0') path = "/";
  if (!idx || *idx < 0 || nlen == 0) return -1;

  rc = lfs_dir_open(&lfs, &dir, path);
  if (rc < 0) return rc;

  skip = *idx;
  i = 0;
  for (;;) {
    rc = lfs_dir_read(&lfs, &dir, &info);
    if (rc < 0) { lfs_dir_close(&lfs, &dir); return rc; }
    if (rc == 0) { lfs_dir_close(&lfs, &dir); return 1; }
    if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0)
      continue;
    if (i == skip) {
      strncpy(name, info.name, nlen - 1);
      name[nlen - 1] = '\0';
      *idx = skip + 1;
      lfs_dir_close(&lfs, &dir);
      return 0;
    }
    i++;
  }
}

static const struct vfs_file_ops g_lfs_ops = {
  lp_open, lp_read, lp_write, lp_close, lp_stat, lp_lseek, lp_listdir
};

const struct vfs_file_ops *lfs_port_ops(void)
{
  return &g_lfs_ops;
}

int lfs_port_mount(const char *mount_path)
{
  int rc;

  if (mounted) return -1;
  memset(&cfg, 0, sizeof(cfg));
  cfg.read  = bd_read;
  cfg.prog  = bd_prog;
  cfg.erase = bd_erase;
  cfg.sync  = bd_sync;
  cfg.read_size = LFS_PORT_BLOCK_SIZE;
  cfg.prog_size = LFS_PORT_BLOCK_SIZE;
  cfg.block_size = LFS_PORT_BLOCK_SIZE;
  cfg.block_count = LFS_PORT_BLOCK_COUNT;
  cfg.cache_size = LFS_PORT_BLOCK_SIZE;
  cfg.lookahead_size = sizeof(look_buf);
  cfg.block_cycles = 100;
  cfg.read_buffer = read_buf;
  cfg.prog_buffer = prog_buf;
  cfg.lookahead_buffer = look_buf;

  memset(bd_mem, 0xff, sizeof(bd_mem));

  rc = lfs_mount(&lfs, &cfg);
  if (rc < 0) {
    rc = lfs_format(&lfs, &cfg);
    if (rc < 0) return rc;
    rc = lfs_mount(&lfs, &cfg);
    if (rc < 0) return rc;
  }
  rc = vfs_mount(mount_path, &g_lfs_ops, &lfs);
  if (rc < 0) {
    lfs_unmount(&lfs);
    return rc;
  }
  strncpy(mount_path_saved, mount_path, VFS_PATH_MAX - 1);
  mount_path_saved[VFS_PATH_MAX - 1] = '\0';
  mounted = 1;
  return 0;
}

int lfs_port_umount(void)
{
  if (!mounted) return -1;
  vfs_umount(mount_path_saved);
  lfs_unmount(&lfs);
  mounted = 0;
  return 0;
}
