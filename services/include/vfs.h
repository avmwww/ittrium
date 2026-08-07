#ifndef ITT_VFS_H
#define ITT_VFS_H

#include <stddef.h>
#include <stdint.h>

#define VFS_MAX_MOUNTS  8
#define VFS_MAX_FD      16
#define VFS_PATH_MAX    128
#define VFS_NAME_MAX    64

#define VFS_TYPE_REG    1
#define VFS_TYPE_DIR    2

#define VFS_O_RDONLY    0x0001
#define VFS_O_WRONLY    0x0002
#define VFS_O_RDWR      0x0003
#define VFS_O_CREAT     0x0100
#define VFS_O_TRUNC     0x0200
#define VFS_O_APPEND    0x0400

#define VFS_SEEK_SET    0
#define VFS_SEEK_CUR    1
#define VFS_SEEK_END    2

struct vfs_stat {
  int type;
  size_t size;
};

struct vfs_file_ops {
  int (*open)(void *fs, const char *rel, int flags, void **out);
  int (*read)(void *file, void *buf, size_t len);
  int (*write)(void *file, const void *buf, size_t len);
  int (*close)(void *file);
  int (*stat)(void *fs, const char *rel, struct vfs_stat *st);
  long (*lseek)(void *file, long off, int whence);
  /* idx in/out: start 0; returns 0+name, 1=end, <0 error */
  int (*listdir)(void *fs, const char *rel, char *name, size_t nlen, int *idx);
};

void vfs_init(void);
int vfs_mount(const char *path, const struct vfs_file_ops *ops, void *fs);
int vfs_umount(const char *path);
int vfs_open(const char *path, int flags);
int vfs_read(int fd, void *buf, size_t len);
int vfs_write(int fd, const void *buf, size_t len);
int vfs_close(int fd);
long vfs_lseek(int fd, long off, int whence);
int vfs_stat(const char *path, struct vfs_stat *st);
int vfs_listdir(const char *path, char *name, size_t nlen, int *idx);

int vfs_mount_count(void);
const char *vfs_mount_path(int i);

#endif
