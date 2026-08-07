#ifndef ITT_RAMFS_H
#define ITT_RAMFS_H

#include "vfs.h"

/* Simple in-memory RW filesystem for bring-up (until littlefs is stable). */
int ramfs_mount(const char *path);

#endif
