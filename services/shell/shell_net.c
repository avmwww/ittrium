#include "shell.h"

#include "lwip/opt.h"

#if LWIP_IPV4

#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "lwip/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/inet_chksum.h"
#include "lwip/sys.h"
#include "lwip/prot/ip4.h"
#include <string.h>

static void print_ip4(const ip4_addr_t *a)
{
  char buf[IP4ADDR_STRLEN_MAX];
  if (!a) {
    shell_puts("0.0.0.0");
    return;
  }
  ip4addr_ntoa_r(a, buf, sizeof(buf));
  shell_puts(buf);
}

static void print_mac(const u8_t *m)
{
  unsigned i;
  for (i = 0; i < 6; i++) {
    static const char *h = "0123456789abcdef";
    if (i)
      shell_putc(':');
    shell_putc(h[(m[i] >> 4) & 0xf]);
    shell_putc(h[m[i] & 0xf]);
  }
}

static void print_netif(struct netif *nif)
{
  char name[3];

  name[0] = nif->name[0];
  name[1] = nif->name[1];
  name[2] = '\0';
  shell_puts(name);
  shell_printf("%u: ", (unsigned)nif->num);
  if (netif_is_up(nif))
    shell_puts("UP ");
  else
    shell_puts("DOWN ");
  if (netif_is_link_up(nif))
    shell_puts("LINK ");
  shell_puts("mtu=");
  shell_printf("%u\r\n", (unsigned)nif->mtu);

  shell_puts("  inet ");
  print_ip4(netif_ip4_addr(nif));
  shell_puts(" netmask ");
  print_ip4(netif_ip4_netmask(nif));
  shell_puts(" gw ");
  print_ip4(netif_ip4_gw(nif));
  shell_puts("\r\n");

  if (nif->hwaddr_len == 6) {
    shell_puts("  ether ");
    print_mac(nif->hwaddr);
    shell_puts("\r\n");
  }
}

static int netif_name_match(struct netif *nif, const char *s)
{
  unsigned n = 0;
  unsigned i = 0;

  if (!s || !s[0] || !s[1])
    return 0;
  if (s[0] != nif->name[0] || s[1] != nif->name[1])
    return 0;
  if (s[2] == '\0')
    return nif->num == 0;
  while (s[2 + i] >= '0' && s[2 + i] <= '9') {
    n = n * 10u + (unsigned)(s[2 + i] - '0');
    if (n > 255u)
      return 0;
    i++;
  }
  return (s[2 + i] == '\0') && (nif->num == (u8_t)n);
}

static struct netif *netif_by_name(const char *s)
{
  struct netif *nif;

  NETIF_FOREACH(nif) {
    if (netif_name_match(nif, s))
      return nif;
  }
  return (struct netif *)0;
}

static int cmd_ifconfig(int argc, char **argv)
{
  struct netif *nif;
  ip4_addr_t ip, mask, gw;
  int have_ip = 0, have_mask = 0, have_gw = 0;
  int do_up = 0, do_down = 0;
  int i;

  if (argc <= 1) {
    LOCK_TCPIP_CORE();
    NETIF_FOREACH(nif)
      print_netif(nif);
    UNLOCK_TCPIP_CORE();
    return 0;
  }

  LOCK_TCPIP_CORE();
  nif = netif_by_name(argv[1]);
  if (!nif) {
    UNLOCK_TCPIP_CORE();
    shell_puts("ifconfig: no such iface\r\n");
    return -1;
  }

  if (argc == 2) {
    print_netif(nif);
    UNLOCK_TCPIP_CORE();
    return 0;
  }

  ip4_addr_set_u32(&ip, ip4_addr_get_u32(netif_ip4_addr(nif)));
  ip4_addr_set_u32(&mask, ip4_addr_get_u32(netif_ip4_netmask(nif)));
  ip4_addr_set_u32(&gw, ip4_addr_get_u32(netif_ip4_gw(nif)));

  for (i = 2; i < argc; i++) {
    if (strcmp(argv[i], "up") == 0) {
      do_up = 1;
    } else if (strcmp(argv[i], "down") == 0) {
      do_down = 1;
    } else if (strcmp(argv[i], "netmask") == 0) {
      if (++i >= argc || !ip4addr_aton(argv[i], &mask)) {
        UNLOCK_TCPIP_CORE();
        shell_puts("ifconfig: bad netmask\r\n");
        return -1;
      }
      have_mask = 1;
    } else if (strcmp(argv[i], "gw") == 0 || strcmp(argv[i], "gateway") == 0) {
      if (++i >= argc || !ip4addr_aton(argv[i], &gw)) {
        UNLOCK_TCPIP_CORE();
        shell_puts("ifconfig: bad gw\r\n");
        return -1;
      }
      have_gw = 1;
    } else if (ip4addr_aton(argv[i], &ip)) {
      have_ip = 1;
    } else {
      UNLOCK_TCPIP_CORE();
      shell_puts("ifconfig: usage: ifconfig [iface [ip] [netmask m] [gw g] [up|down]]\r\n");
      return -1;
    }
  }

  if (have_ip || have_mask || have_gw)
    netif_set_addr(nif, &ip, &mask, &gw);
  if (do_down)
    netif_set_down(nif);
  if (do_up)
    netif_set_up(nif);

  print_netif(nif);
  UNLOCK_TCPIP_CORE();
  return 0;
}

static int cmd_arp(int argc, char **argv)
{
  size_t i;
  ip4_addr_t *ip;
  struct netif *nif;
  struct eth_addr *eth;

  (void)argc;
  (void)argv;

  shell_puts("IP               MAC\r\n");
  LOCK_TCPIP_CORE();
  for (i = 0; i < ARP_TABLE_SIZE; i++) {
    if (!etharp_get_entry(i, &ip, &nif, &eth))
      continue;
    print_ip4(ip);
    shell_puts("  ");
    print_mac(eth->addr);
    shell_puts("\r\n");
  }
  UNLOCK_TCPIP_CORE();
  return 0;
}

#if LWIP_RAW && LWIP_ICMP

struct ping_ctx {
  sys_sem_t sem;
  u16_t seq;
  volatile int got;
  ip4_addr_t dst;
};

static u8_t ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p,
                      const ip_addr_t *addr)
{
  struct ping_ctx *ctx = (struct ping_ctx *)arg;
  struct icmp_echo_hdr *iecho;

  (void)pcb;
  (void)addr;
  if (!ctx || !p)
    return 0;

  if (p->tot_len < (u16_t)(IP_HLEN + sizeof(struct icmp_echo_hdr)))
    return 0;
  if (pbuf_remove_header(p, IP_HLEN) != 0)
    return 0;

  iecho = (struct icmp_echo_hdr *)p->payload;
  if (iecho->type == ICMP_ER && iecho->id == 0x4954 /* 'IT' */ &&
      iecho->seqno == lwip_htons(ctx->seq)) {
    ctx->got = 1;
    sys_sem_signal(&ctx->sem);
    pbuf_free(p);
    return 1;
  }
  pbuf_add_header(p, IP_HLEN);
  return 0;
}

static int ping_send(struct raw_pcb *pcb, const ip4_addr_t *dst, u16_t seq)
{
  struct pbuf *p;
  struct icmp_echo_hdr *iecho;
  size_t i;

  p = pbuf_alloc(PBUF_IP, (u16_t)(sizeof(struct icmp_echo_hdr) + 32), PBUF_RAM);
  if (!p)
    return -1;
  iecho = (struct icmp_echo_hdr *)p->payload;
  ICMPH_TYPE_SET(iecho, ICMP_ECHO);
  ICMPH_CODE_SET(iecho, 0);
  iecho->chksum = 0;
  iecho->id = 0x4954;
  iecho->seqno = lwip_htons(seq);
  for (i = 0; i < 32; i++)
    ((u8_t *)iecho)[sizeof(*iecho) + i] = (u8_t)i;
  iecho->chksum = inet_chksum(iecho, p->len);

  {
    ip_addr_t tip;
    ip_addr_copy_from_ip4(tip, *dst);
    if (raw_sendto(pcb, p, &tip) != ERR_OK) {
      pbuf_free(p);
      return -1;
    }
  }
  pbuf_free(p);
  return 0;
}

static int parse_ip4(const char *s, ip4_addr_t *out)
{
  return ip4addr_aton(s, out) ? 0 : -1;
}

static int cmd_ping(int argc, char **argv)
{
  struct ping_ctx ctx;
  struct raw_pcb *pcb;
  int n, ok = 0;
  u32_t t0, t1;

  if (argc < 2) {
    shell_puts("usage: ping <ip>\r\n");
    return -1;
  }
  if (parse_ip4(argv[1], &ctx.dst) != 0) {
    shell_puts("ping: bad address\r\n");
    return -1;
  }
  if (sys_sem_new(&ctx.sem, 0) != ERR_OK) {
    shell_puts("ping: sem\r\n");
    return -1;
  }

  LOCK_TCPIP_CORE();
  pcb = raw_new(IP_PROTO_ICMP);
  if (!pcb) {
    UNLOCK_TCPIP_CORE();
    sys_sem_free(&ctx.sem);
    shell_puts("ping: raw pcb\r\n");
    return -1;
  }
  raw_recv(pcb, ping_recv, &ctx);
  raw_bind(pcb, IP_ADDR_ANY);
  UNLOCK_TCPIP_CORE();

  shell_puts("PING ");
  print_ip4(&ctx.dst);
  shell_puts("\r\n");

  for (n = 1; n <= 4; n++) {
    ctx.seq = (u16_t)n;
    ctx.got = 0;
    LOCK_TCPIP_CORE();
    if (ping_send(pcb, &ctx.dst, ctx.seq) != 0) {
      UNLOCK_TCPIP_CORE();
      shell_puts("ping: send failed\r\n");
      break;
    }
    UNLOCK_TCPIP_CORE();

    t0 = sys_now();
    if (sys_arch_sem_wait(&ctx.sem, 1000) == SYS_ARCH_TIMEOUT) {
      shell_printf("seq=%u timeout\r\n", (unsigned)n);
    } else {
      t1 = sys_now();
      shell_printf("seq=%u time=%ums\r\n", (unsigned)n, (unsigned)(t1 - t0));
      ok++;
    }
  }

  LOCK_TCPIP_CORE();
  raw_remove(pcb);
  UNLOCK_TCPIP_CORE();
  sys_sem_free(&ctx.sem);
  shell_printf("%u received\r\n", (unsigned)ok);
  return ok ? 0 : -1;
}

#endif /* LWIP_RAW && LWIP_ICMP */

void shell_net_register(void)
{
  shell_register("ifconfig", "show/set iface (ip netmask gw up|down)", cmd_ifconfig);
  shell_register("arp", "show ARP table", cmd_arp);
#if LWIP_RAW && LWIP_ICMP
  shell_register("ping", "ICMP echo (ping <ip>)", cmd_ping);
#endif
}

#else /* !LWIP_IPV4 */

void shell_net_register(void)
{
}

#endif
