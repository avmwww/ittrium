#ifndef ITT_ROMFS_H
#define ITT_ROMFS_H

#include <stddef.h>
#include <stdint.h>
#include "vfs.h"

#define ROMFS_NAME_MAX  48
#define ROMFS_MAX_ENT   32

struct romfs_entry {
  char name[ROMFS_NAME_MAX];
  const void *data;
  size_t size;
};

struct romfs {
  struct romfs_entry *ents;
  int nents;
  int capacity;
  int owned; /* 1 if ents allocated for builtins */
};

/* entries=NULL,n=0 mounts the builtin table filled via romfs_add_builtin */
int romfs_mount(const char *path, struct romfs_entry *entries, int n);
int romfs_add_builtin(const char *name, const void *data, size_t size);
const struct vfs_file_ops *romfs_ops(void);

#endif
