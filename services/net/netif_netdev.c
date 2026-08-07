#include "lwip/opt.h"

#if LWIP_ETHERNET

#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "lwip/snmp.h"
#include "lwip/tcpip.h"
#include "netif/ethernet.h"
#include "drv/netdev.h"
#include <string.h>

#ifndef NETIF_HOSTNAME
#define NETIF_HOSTNAME "ittrium"
#endif

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  struct netdev *dev = (struct netdev *)netif->state;
  uint8_t buf[1514];
  uint16_t len = 0;
  struct pbuf *q;

  for (q = p; q != NULL; q = q->next) {
    if (len + q->len > sizeof(buf))
      return ERR_BUF;
    memcpy(buf + len, q->payload, q->len);
    len = (uint16_t)(len + q->len);
  }
  if (netdev_tx(dev, buf, len) < 0)
    return ERR_IF;
  return ERR_OK;
}

static void netdev_rx_tcpip(struct netdev *dev, const void *data, size_t len,
                            void *arg)
{
  /* IRQ context → defer via tcpip copy path: allocate and post */
  struct netif *netif = (struct netif *)arg;
  struct pbuf *p;

  (void)dev;
  if (!netif || !data || len == 0 || len > 0xffffu)
    return;
  p = pbuf_alloc(PBUF_RAW, (u16_t)len, PBUF_POOL);
  if (!p)
    return;
  if (pbuf_take(p, data, (u16_t)len) != ERR_OK) {
    pbuf_free(p);
    return;
  }
  if (tcpip_input(p, netif) != ERR_OK)
    pbuf_free(p);
}

static err_t netif_netdev_init(struct netif *netif)
{
  struct netdev *dev = (struct netdev *)netif->state;

  netif->name[0] = 'e';
  netif->name[1] = 'n';
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  netif->hwaddr_len = 6;
  memcpy(netif->hwaddr, dev->mac, 6);
  netif->mtu = 1500;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
  netdev_set_rx(dev, netdev_rx_tcpip, netif);
  return ERR_OK;
}

err_t netif_netdev_add(struct netif *netif, struct netdev *dev,
                       const ip4_addr_t *ip, const ip4_addr_t *mask,
                       const ip4_addr_t *gw)
{
  if (!netif || !dev)
    return ERR_ARG;
  if (!netif_add(netif, ip, mask, gw, dev, netif_netdev_init, tcpip_input))
    return ERR_IF;
  netif_set_up(netif);
  return ERR_OK;
}

#else

typedef int netif_netdev_dummy;

#endif
