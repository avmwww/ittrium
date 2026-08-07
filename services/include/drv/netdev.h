#ifndef ITT_DRV_NETDEV_H
#define ITT_DRV_NETDEV_H

#include <stddef.h>
#include <stdint.h>

struct netdev;

typedef void (*netdev_rx_fn)(struct netdev *dev, const void *data, size_t len,
                             void *arg);

struct netdev_ops {
  int (*init)(struct netdev *dev);
  int (*start)(struct netdev *dev);
  int (*stop)(struct netdev *dev);
  int (*tx)(struct netdev *dev, const void *data, size_t len);
  int (*set_mac)(struct netdev *dev, const uint8_t mac[6]);
};

struct netdev {
  const char *name;
  const struct netdev_ops *ops;
  void *priv;
  uint8_t mac[6];
  netdev_rx_fn rx;
  void *rx_arg;
};

int netdev_register(struct netdev *dev);
struct netdev *netdev_get(const char *name);
int netdev_init(struct netdev *dev);
int netdev_start(struct netdev *dev);
int netdev_tx(struct netdev *dev, const void *data, size_t len);
void netdev_set_rx(struct netdev *dev, netdev_rx_fn fn, void *arg);

#endif
