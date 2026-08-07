#ifndef ITT_SYSFS_H
#define ITT_SYSFS_H

#include "vfs.h"

int sysfs_mount(const char *path);
const struct vfs_file_ops *sysfs_ops(void);

#endif
