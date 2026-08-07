#include "drv/device.h"
#include <string.h>

struct itt_devices g_ittrium_devices;

int itt_device_add(const char *name, enum itt_dev_class class, void *drv)
{
  unsigned i;

  if (!name || !drv || class == 0)
    return -1;
  if (g_ittrium_devices.n >= ITT_DEVICES_MAX)
    return -1;
  for (i = 0; i < g_ittrium_devices.n; i++) {
    if (strcmp(g_ittrium_devices.slot[i].name, name) == 0)
      return -1;
  }
  g_ittrium_devices.slot[g_ittrium_devices.n].name = name;
  g_ittrium_devices.slot[g_ittrium_devices.n].class = class;
  g_ittrium_devices.slot[g_ittrium_devices.n].drv = drv;
  g_ittrium_devices.n++;
  return 0;
}

struct itt_device *itt_device_find(const char *name)
{
  unsigned i;

  if (!name)
    return 0;
  for (i = 0; i < g_ittrium_devices.n; i++) {
    if (strcmp(g_ittrium_devices.slot[i].name, name) == 0)
      return &g_ittrium_devices.slot[i];
  }
  return 0;
}

struct itt_device *itt_device_find_class(enum itt_dev_class class, unsigned idx)
{
  unsigned i;
  unsigned hit = 0;

  for (i = 0; i < g_ittrium_devices.n; i++) {
    if (g_ittrium_devices.slot[i].class != class)
      continue;
    if (hit == idx)
      return &g_ittrium_devices.slot[i];
    hit++;
  }
  return 0;
}
