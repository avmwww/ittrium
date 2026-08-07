#include "vfs.h"
#include <string.h>

struct vfs_fd {
  int used;
  const struct vfs_file_ops *ops;
  void *file;
};

static struct {
  char path[VFS_PATH_MAX];
  const struct vfs_file_ops *ops;
  void *fs;
  int used;
  int plen;
} mounts[VFS_MAX_MOUNTS];

static struct vfs_fd fds[VFS_MAX_FD];

static int path_len(const char *p)
{
  int n = 0;
  while (p[n]) n++;
  return n;
}

static int path_eq_prefix(const char *path, const char *mnt, int mlen)
{
  int i;
  if (mlen <= 0) return 0;
  for (i = 0; i < mlen; i++) {
    if (path[i] != mnt[i]) return 0;
  }
  /* mount "/" matches everything; else need / or end after prefix */
  if (mlen == 1 && mnt[0] == '/') return 1;
  return path[mlen] == '\0' || path[mlen] == '/';
}

/* Find longest matching mount; set *rel to path relative to mount ("" or "foo") */
static int find_mount(const char *path, const char **rel)
{
  int best = -1, best_len = -1, i;

  if (!path || path[0] != '/') return -1;

  for (i = 0; i < VFS_MAX_MOUNTS; i++) {
    if (!mounts[i].used) continue;
    if (!path_eq_prefix(path, mounts[i].path, mounts[i].plen)) continue;
    if (mounts[i].plen > best_len) {
      best_len = mounts[i].plen;
      best = i;
    }
  }
  if (best < 0) return -1;

  if (best_len == 1 && mounts[best].path[0] == '/')
    *rel = path + 1;
  else if (path[best_len] == '/')
    *rel = path + best_len + 1;
  else
    *rel = path + best_len; /* exact mount point -> "" */

  return best;
}

void vfs_init(void)
{
  memset(mounts, 0, sizeof(mounts));
  memset(fds, 0, sizeof(fds));
}

int vfs_mount(const char *path, const struct vfs_file_ops *ops, void *fs)
{
  int i, plen;

  if (!path || path[0] != '/' || !ops) return -1;
  plen = path_len(path);
  if (plen >= VFS_PATH_MAX) return -1;

  for (i = 0; i < VFS_MAX_MOUNTS; i++) {
    if (mounts[i].used && mounts[i].plen == plen &&
        memcmp(mounts[i].path, path, (size_t)plen) == 0)
      return -1;
  }
  for (i = 0; i < VFS_MAX_MOUNTS; i++) {
    if (mounts[i].used) continue;
    memcpy(mounts[i].path, path, (size_t)plen + 1);
    mounts[i].ops = ops;
    mounts[i].fs = fs;
    mounts[i].plen = plen;
    mounts[i].used = 1;
    return 0;
  }
  return -1;
}

int vfs_umount(const char *path)
{
  int i, plen;
  if (!path) return -1;
  plen = path_len(path);
  for (i = 0; i < VFS_MAX_MOUNTS; i++) {
    if (!mounts[i].used) continue;
    if (mounts[i].plen == plen &&
        memcmp(mounts[i].path, path, (size_t)plen) == 0) {
      mounts[i].used = 0;
      return 0;
    }
  }
  return -1;
}

static int alloc_fd(void)
{
  int i;
  for (i = 0; i < VFS_MAX_FD; i++) {
    if (!fds[i].used) return i;
  }
  return -1;
}

int vfs_open(const char *path, int flags)
{
  const char *rel;
  int mi, fd, rc;
  void *file = 0;

  mi = find_mount(path, &rel);
  if (mi < 0) return -1;
  if (!mounts[mi].ops->open) return -1;

  fd = alloc_fd();
  if (fd < 0) return -1;

  rc = mounts[mi].ops->open(mounts[mi].fs, rel, flags, &file);
  if (rc < 0) return rc;

  fds[fd].used = 1;
  fds[fd].ops = mounts[mi].ops;
  fds[fd].file = file;
  return fd;
}

int vfs_read(int fd, void *buf, size_t len)
{
  if (fd < 0 || fd >= VFS_MAX_FD || !fds[fd].used) return -1;
  if (!fds[fd].ops->read) return -1;
  return fds[fd].ops->read(fds[fd].file, buf, len);
}

int vfs_write(int fd, const void *buf, size_t len)
{
  if (fd < 0 || fd >= VFS_MAX_FD || !fds[fd].used) return -1;
  if (!fds[fd].ops->write) return -1;
  return fds[fd].ops->write(fds[fd].file, buf, len);
}

int vfs_close(int fd)
{
  int rc = 0;
  if (fd < 0 || fd >= VFS_MAX_FD || !fds[fd].used) return -1;
  if (fds[fd].ops->close)
    rc = fds[fd].ops->close(fds[fd].file);
  fds[fd].used = 0;
  fds[fd].file = 0;
  return rc;
}

long vfs_lseek(int fd, long off, int whence)
{
  if (fd < 0 || fd >= VFS_MAX_FD || !fds[fd].used) return -1;
  if (!fds[fd].ops->lseek) return -1;
  return fds[fd].ops->lseek(fds[fd].file, off, whence);
}

int vfs_stat(const char *path, struct vfs_stat *st)
{
  const char *rel;
  int mi;
  mi = find_mount(path, &rel);
  if (mi < 0) return -1;
  if (!mounts[mi].ops->stat) return -1;
  return mounts[mi].ops->stat(mounts[mi].fs, rel, st);
}

int vfs_listdir(const char *path, char *name, size_t nlen, int *idx)
{
  const char *rel;
  int mi;
  mi = find_mount(path, &rel);
  if (mi < 0) return -1;
  if (!mounts[mi].ops->listdir) return -1;
  return mounts[mi].ops->listdir(mounts[mi].fs, rel, name, nlen, idx);
}

int vfs_mount_count(void)
{
  int i, n = 0;
  for (i = 0; i < VFS_MAX_MOUNTS; i++)
    if (mounts[i].used) n++;
  return n;
}

const char *vfs_mount_path(int i)
{
  int j, n = 0;
  for (j = 0; j < VFS_MAX_MOUNTS; j++) {
    if (!mounts[j].used) continue;
    if (n == i) return mounts[j].path;
    n++;
  }
  return 0;
}
