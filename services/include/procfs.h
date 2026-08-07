#ifndef ITT_PROCFS_H
#define ITT_PROCFS_H

#include "vfs.h"

int procfs_mount(const char *path);
const struct vfs_file_ops *procfs_ops(void);

#endif
