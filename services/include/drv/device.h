#ifndef ITT_DRV_DEVICE_H
#define ITT_DRV_DEVICE_H

#define ITT_DEVICES_MAX 8

enum itt_dev_class {
  ITT_DEV_CONSOLE = 1,
  ITT_DEV_NET = 2,
  /* ITT_DEV_BLK later */
};

struct itt_device {
  const char *name;
  enum itt_dev_class class;
  void *drv; /* console_ops* | netdev* | … */
};

struct itt_devices {
  struct itt_device slot[ITT_DEVICES_MAX];
  unsigned n;
};

extern struct itt_devices g_ittrium_devices;

int itt_device_add(const char *name, enum itt_dev_class class, void *drv);
struct itt_device *itt_device_find(const char *name);
struct itt_device *itt_device_find_class(enum itt_dev_class class, unsigned idx);

#endif
