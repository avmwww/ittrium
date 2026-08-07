#ifndef ITT_LFS_PORT_H
#define ITT_LFS_PORT_H

#include "vfs.h"

#define LFS_PORT_BLOCK_SIZE  256
#define LFS_PORT_BLOCK_COUNT 256  /* 64 KiB RAM BD */

int lfs_port_mount(const char *mount_path);
int lfs_port_umount(void);
const struct vfs_file_ops *lfs_port_ops(void);

#endif
