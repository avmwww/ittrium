#include "drv/netdev.h"
#include "drv/device.h"
#include <string.h>

int netdev_register(struct netdev *dev)
{
  if (!dev || !dev->name || !dev->ops)
    return -1;
  return itt_device_add(dev->name, ITT_DEV_NET, dev);
}

struct netdev *netdev_get(const char *name)
{
  struct itt_device *d = itt_device_find(name);

  if (!d || d->class != ITT_DEV_NET)
    return 0;
  return (struct netdev *)d->drv;
}

int netdev_init(struct netdev *dev)
{
  if (!dev || !dev->ops || !dev->ops->init)
    return -1;
  return dev->ops->init(dev);
}

int netdev_start(struct netdev *dev)
{
  if (!dev || !dev->ops || !dev->ops->start)
    return -1;
  return dev->ops->start(dev);
}

int netdev_tx(struct netdev *dev, const void *data, size_t len)
{
  if (!dev || !dev->ops || !dev->ops->tx)
    return -1;
  return dev->ops->tx(dev, data, len);
}

void netdev_set_rx(struct netdev *dev, netdev_rx_fn fn, void *arg)
{
  if (!dev)
    return;
  dev->rx = fn;
  dev->rx_arg = arg;
}
