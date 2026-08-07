#ifndef ITT_NETIF_NETDEV_H
#define ITT_NETIF_NETDEV_H

#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "drv/netdev.h"

err_t netif_netdev_add(struct netif *netif, struct netdev *dev,
                       const ip4_addr_t *ip, const ip4_addr_t *mask,
                       const ip4_addr_t *gw);

#endif
