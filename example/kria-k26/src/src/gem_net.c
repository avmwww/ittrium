/**
 * Cadence GEM3 (ZynqMP) → netdev_ops.
 * 64-bit BDs (AArch64), IRQ RX → task, polled TX complete.
 * PHY: Marvell 88E1512 @ GEM_PHY_ADDR (RGMII); assumes FSBL left clocks/MIO/reset.
 */
#include "ittrium.h"
#include "target.h"
#include "drv/netdev.h"
#include <string.h>

#define GEM_NWCTRL     (*(volatile uint32_t *)(GEM_BASE + 0x000))
#define GEM_NWCFG      (*(volatile uint32_t *)(GEM_BASE + 0x004))
#define GEM_NWSR       (*(volatile uint32_t *)(GEM_BASE + 0x008))
#define GEM_DMACR      (*(volatile uint32_t *)(GEM_BASE + 0x010))
#define GEM_TXSR       (*(volatile uint32_t *)(GEM_BASE + 0x014))
#define GEM_RXQBASE    (*(volatile uint32_t *)(GEM_BASE + 0x018))
#define GEM_TXQBASE    (*(volatile uint32_t *)(GEM_BASE + 0x01C))
#define GEM_RXSR       (*(volatile uint32_t *)(GEM_BASE + 0x020))
#define GEM_ISR        (*(volatile uint32_t *)(GEM_BASE + 0x024))
#define GEM_IER        (*(volatile uint32_t *)(GEM_BASE + 0x028))
#define GEM_IDR        (*(volatile uint32_t *)(GEM_BASE + 0x02C))
#define GEM_PHYMNTNC   (*(volatile uint32_t *)(GEM_BASE + 0x034))
#define GEM_LADDR1L    (*(volatile uint32_t *)(GEM_BASE + 0x088))
#define GEM_LADDR1H    (*(volatile uint32_t *)(GEM_BASE + 0x08C))
#define GEM_TXQBASE_HI (*(volatile uint32_t *)(GEM_BASE + 0x4C8))
#define GEM_RXQBASE_HI (*(volatile uint32_t *)(GEM_BASE + 0x4D4))

#define NWCTRL_MDEN    (1u << 4)
#define NWCTRL_TXEN    (1u << 3)
#define NWCTRL_RXEN    (1u << 2)
#define NWCTRL_STARTTX (1u << 9)
#define NWCTRL_STATCLR (1u << 5)

#define NWCFG_FDEN     (1u << 1)
#define NWCFG_1000     (1u << 10)
#define NWCFG_100      (1u << 0)
#define NWCFG_1536RXEN (1u << 8)
#define NWCFG_FCSREM   (1u << 17)
#define NWCFG_LENERR   (1u << 16)
#define NWCFG_RXCHKSUM (1u << 24)
#define NWCFG_DWIDTH64 (1u << 21)
#define NWCFG_MDC_DIV48 (0x3u << 18)

#define NWSR_MDIOIDLE  (1u << 2)

#define DMACR_RXBUF_SHIFT 16
#define DMACR_INCR16      0x10u
#define DMACR_RXSIZE      (0x3u << 8)
#define DMACR_TXSIZE      (1u << 10)
#define DMACR_TCPCKSUM    (1u << 11)
#define DMACR_ADDR64      (1u << 30)

#define IXR_FRAMERX    (1u << 1)
#define IXR_RXUSED     (1u << 2)
#define IXR_TXCOMPL    (1u << 7)
#define IXR_RXOVR      (1u << 10)

#define TXBD_USED      (1u << 31)
#define TXBD_WRAP      (1u << 30)
#define TXBD_LAST      (1u << 15)
#define TXBD_LEN_MASK  0x3fffu

#define RXBD_WRAP      (1u << 1)
#define RXBD_NEW       (1u << 0)
#define RXBD_EOF       (1u << 15)
#define RXBD_SOF       (1u << 14)
#define RXBD_LEN_MASK  0x1fffu
#define RXBD_ADDR_MASK 0xfffffffcu

#define PHY_OP_READ    0x20000000u
#define PHY_OP_WRITE   0x10000000u
#define PHY_CLAUSE22   0x40000000u

#define RX_N           16u
#define TX_N           8u
#define BUF_SIZE       1536u
#define GEM_FLG_RX     (1u << 0)

struct gem_bd {
  uint32_t addr;
  uint32_t status;
  uint32_t addr_hi;
  uint32_t reserved;
} __attribute__((aligned(64)));

static struct gem_bd rx_bd[RX_N] __attribute__((aligned(64)));
static struct gem_bd tx_bd[TX_N] __attribute__((aligned(64)));
static uint8_t rx_buf[RX_N][BUF_SIZE] __attribute__((aligned(64)));
static uint8_t tx_buf[TX_N][BUF_SIZE] __attribute__((aligned(64)));

static unsigned rx_idx;
static unsigned tx_idx;
static struct netdev g_dev;
static int g_ready;
static uint64_t gem_tsk_stack[GEM_NET_TSK_STACK_SIZE / sizeof(uint64_t)];

static void gem_mdio_wait(void)
{
  unsigned n = 100000;
  while (!(GEM_NWSR & NWSR_MDIOIDLE) && n--)
    ;
}

static uint16_t gem_phy_read(unsigned phy, unsigned reg)
{
  gem_mdio_wait();
  GEM_PHYMNTNC = PHY_CLAUSE22 | PHY_OP_READ |
                 ((phy & 0x1fu) << 23) | ((reg & 0x1fu) << 18);
  gem_mdio_wait();
  return (uint16_t)(GEM_PHYMNTNC & 0xffffu);
}

static void gem_phy_write(unsigned phy, unsigned reg, uint16_t val)
{
  gem_mdio_wait();
  GEM_PHYMNTNC = PHY_CLAUSE22 | PHY_OP_WRITE |
                 ((phy & 0x1fu) << 23) | ((reg & 0x1fu) << 18) | val;
  gem_mdio_wait();
}

/* BMCR / BMSR / ANAR — enough for 1G RGMII Marvell */
static int gem_phy_autoneg(void)
{
  unsigned phy = GEM_PHY_ADDR;
  unsigned i;
  uint16_t bmsr, speed;

  /* advertise 1000/100/10 FD */
  gem_phy_write(phy, 4, 0x01e1);
  gem_phy_write(phy, 9, 0x0200);
  gem_phy_write(phy, 0, 0x1200); /* AN enable + restart */

  for (i = 0; i < 50; i++) {
    bmsr = gem_phy_read(phy, 1);
    if (bmsr & 0x0020) /* AN complete */
      break;
    dly_tsk(100);
  }
  if (!(bmsr & 0x0020))
    return -1;

  /* Marvell specific status 0x11 */
  speed = gem_phy_read(phy, 0x11);
  if (((speed >> 14) & 3u) == 2u) {
    GEM_NWCFG = (GEM_NWCFG & ~(NWCFG_100 | NWCFG_1000)) | NWCFG_1000 | NWCFG_FDEN;
  } else if (((speed >> 14) & 3u) == 1u) {
    GEM_NWCFG = (GEM_NWCFG & ~(NWCFG_100 | NWCFG_1000)) | NWCFG_100 | NWCFG_FDEN;
  } else {
    GEM_NWCFG = (GEM_NWCFG & ~(NWCFG_100 | NWCFG_1000)) | NWCFG_FDEN;
  }
  return 0;
}

static void gem_set_mac(const uint8_t mac[6])
{
  GEM_LADDR1L = (uint32_t)mac[0] | ((uint32_t)mac[1] << 8) |
                ((uint32_t)mac[2] << 16) | ((uint32_t)mac[3] << 24);
  GEM_LADDR1H = (uint32_t)mac[4] | ((uint32_t)mac[5] << 8);
}

static void gem_rx_rearm(unsigned i)
{
  uint64_t a = (uint64_t)(uintptr_t)rx_buf[i];
  uint32_t lo = (uint32_t)a & RXBD_ADDR_MASK;

  if (i == RX_N - 1u)
    lo |= RXBD_WRAP;
  rx_bd[i].addr = lo;
  rx_bd[i].addr_hi = (uint32_t)(a >> 32);
  rx_bd[i].status = 0;
  rx_bd[i].reserved = 0;
  __asm__ volatile("dmb sy" ::: "memory");
}

static void gem_process_rx(void)
{
  while (rx_bd[rx_idx].addr & RXBD_NEW) {
    uint32_t st = rx_bd[rx_idx].status;
    uint32_t len = st & RXBD_LEN_MASK;

    if ((st & (RXBD_SOF | RXBD_EOF)) == (RXBD_SOF | RXBD_EOF) &&
        len > 0 && g_dev.rx) {
      g_dev.rx(&g_dev, rx_buf[rx_idx], len, g_dev.rx_arg);
    }
    gem_rx_rearm(rx_idx);
    rx_idx = (rx_idx + 1u) % RX_N;
  }
}

static void gem_irq(void)
{
  uint32_t isr = GEM_ISR;

  GEM_ISR = isr;
  if (isr & (IXR_FRAMERX | IXR_RXUSED | IXR_RXOVR))
    iset_flg(GEM_FLG_ID, GEM_FLG_RX);
  if (isr & IXR_TXCOMPL)
    GEM_TXSR = GEM_TXSR;
}

static void gem_rx_task(void *exinf)
{
  FLGPTN ptn;

  (void)exinf;
  for (;;) {
    twai_flg(GEM_FLG_ID, GEM_FLG_RX, TWF_ORW, &ptn, TMO_FEVR);
    clr_flg(GEM_FLG_ID, ~GEM_FLG_RX);
    gem_process_rx();
  }
}

static int gem_start_rx_task(void)
{
  T_CFLG pk_cflg;
  T_CTSK pk;

  pk_cflg.flgatr = TA_TFIFO | TA_CLR | TA_WMUL;
  pk_cflg.iflgptn = 0;
  if (cre_flg(GEM_FLG_ID, &pk_cflg) != E_OK)
    return -1;

  pk.tskatr = TA_HLNG;
  pk.exinf = 0;
  pk.task = (FP)gem_rx_task;
  pk.itskpri = GEM_NET_TASK_PRIO;
  pk.stksz = GEM_NET_TSK_STACK_SIZE;
  pk.stk = gem_tsk_stack;
  if (cre_tsk(GEM_NET_TASK_ID, &pk) != E_OK)
    return -1;
  return act_tsk(GEM_NET_TASK_ID) == E_OK ? 0 : -1;
}

static int gem_hw_init(struct netdev *dev)
{
  unsigned i;
  uint64_t ra, ta;

  (void)dev;
  GEM_NWCTRL = 0;
  GEM_IDR = 0xffffffffu;
  GEM_ISR = 0xffffffffu;
  GEM_TXSR = 0xffffffffu;
  GEM_RXSR = 0xffffffffu;
  GEM_NWCTRL = NWCTRL_STATCLR | NWCTRL_MDEN;

  GEM_NWCFG = NWCFG_FDEN | NWCFG_1000 | NWCFG_1536RXEN | NWCFG_FCSREM |
              NWCFG_LENERR | NWCFG_RXCHKSUM | NWCFG_DWIDTH64 | NWCFG_MDC_DIV48;
  GEM_DMACR = DMACR_ADDR64 | DMACR_TCPCKSUM | DMACR_TXSIZE | DMACR_RXSIZE |
              DMACR_INCR16 | ((BUF_SIZE / 64u) << DMACR_RXBUF_SHIFT);

  for (i = 0; i < RX_N; i++)
    gem_rx_rearm(i);
  for (i = 0; i < TX_N; i++) {
    uint64_t a = (uint64_t)(uintptr_t)tx_buf[i];
    tx_bd[i].addr = (uint32_t)a;
    tx_bd[i].addr_hi = (uint32_t)(a >> 32);
    tx_bd[i].status = TXBD_USED;
    tx_bd[i].reserved = 0;
    if (i == TX_N - 1u)
      tx_bd[i].status |= TXBD_WRAP;
  }
  rx_idx = 0;
  tx_idx = 0;

  ra = (uint64_t)(uintptr_t)rx_bd;
  ta = (uint64_t)(uintptr_t)tx_bd;
  GEM_RXQBASE = (uint32_t)ra;
  GEM_RXQBASE_HI = (uint32_t)(ra >> 32);
  GEM_TXQBASE = (uint32_t)ta;
  GEM_TXQBASE_HI = (uint32_t)(ta >> 32);

  if (dev->mac[0] | dev->mac[1] | dev->mac[2] | dev->mac[3] | dev->mac[4] | dev->mac[5])
    gem_set_mac(dev->mac);
  else {
    static const uint8_t defmac[6] = { 0x00, 0x0a, 0x35, 0x00, 0x22, 0x01 };
    memcpy(dev->mac, defmac, 6);
    gem_set_mac(defmac);
  }

  if (gem_phy_autoneg() != 0) {
    /* keep forced 1G FD — link may come up later */
  }

  return 0;
}

static int gem_hw_start(struct netdev *dev)
{
  (void)dev;
  GEM_IER = IXR_FRAMERX | IXR_RXUSED | IXR_RXOVR | IXR_TXCOMPL;
  install_handler(gem_irq, NETDEV_VEC_NO, GEM_IRQ_PRIO);
  gic_enable_irq(GEM_IRQ, GEM_IRQ_PRIO);
  GEM_NWCTRL |= NWCTRL_TXEN | NWCTRL_RXEN | NWCTRL_MDEN;
  g_ready = 1;
  return 0;
}

static int gem_hw_tx(struct netdev *dev, const void *data, size_t len)
{
  struct gem_bd *bd;
  unsigned n = 0;

  (void)dev;
  if (!g_ready || len == 0 || len > BUF_SIZE)
    return -1;

  bd = &tx_bd[tx_idx];
  while (!(bd->status & TXBD_USED)) {
    if (++n > 100000u)
      return -1;
  }

  memcpy(tx_buf[tx_idx], data, len);
  {
    uint64_t a = (uint64_t)(uintptr_t)tx_buf[tx_idx];
    uint32_t st = (uint32_t)len | TXBD_LAST;
    if (tx_idx == TX_N - 1u)
      st |= TXBD_WRAP;
    bd->addr = (uint32_t)a;
    bd->addr_hi = (uint32_t)(a >> 32);
    __asm__ volatile("dmb sy" ::: "memory");
    bd->status = st; /* clear USED → HW owns */
  }
  __asm__ volatile("dmb sy" ::: "memory");
  GEM_NWCTRL |= NWCTRL_STARTTX;
  tx_idx = (tx_idx + 1u) % TX_N;
  return 0;
}

static int gem_hw_set_mac(struct netdev *dev, const uint8_t mac[6])
{
  memcpy(dev->mac, mac, 6);
  gem_set_mac(mac);
  return 0;
}

static const struct netdev_ops g_ops = {
  .init = gem_hw_init,
  .start = gem_hw_start,
  .tx = gem_hw_tx,
  .set_mac = gem_hw_set_mac,
};

int gem_net_init(void)
{
  if (gem_start_rx_task() != 0)
    return -1;

  g_dev.name = "gem3";
  g_dev.ops = &g_ops;
  g_dev.priv = 0;
  netdev_register(&g_dev);
  if (netdev_init(&g_dev) != 0)
    return -1;
  return netdev_start(&g_dev);
}

struct netdev *gem_net_dev(void)
{
  return &g_dev;
}
