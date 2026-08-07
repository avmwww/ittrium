#include "drv/netdev.h"
#include <string.h>

#define NETDEV_MAX 4

static struct netdev *g_devs[NETDEV_MAX];
static int g_ndev;

int netdev_register(struct netdev *dev)
{
  int i;

  if (!dev || !dev->name || !dev->ops)
    return -1;
  if (g_ndev >= NETDEV_MAX)
    return -1;
  for (i = 0; i < g_ndev; i++) {
    if (strcmp(g_devs[i]->name, dev->name) == 0)
      return -1;
  }
  g_devs[g_ndev++] = dev;
  return 0;
}

struct netdev *netdev_get(const char *name)
{
  int i;

  if (!name)
    return 0;
  for (i = 0; i < g_ndev; i++) {
    if (strcmp(g_devs[i]->name, name) == 0)
      return g_devs[i];
  }
  return 0;
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
