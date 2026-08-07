#include "ittrium.h"
#include "target.h"
#include "drv/netdev.h"
#include <string.h>

#define VIRTIO_MAGIC		0x74726976u
#define VIRTIO_VERSION		2u
#define VIRTIO_DEV_NET		1u

#define VIRTIO_STATUS_ACK	1u
#define VIRTIO_STATUS_DRIVER	2u
#define VIRTIO_STATUS_FEATURES_OK 8u
#define VIRTIO_STATUS_DRIVER_OK	4u
#define VIRTIO_STATUS_FAILED	128u

#define VIRTIO_F_VERSION_1	(1ull << 32)
#define VIRTIO_NET_F_MAC	(1ull << 5)
#define VIRTIO_NET_F_STATUS	(1ull << 16)

#define VRING_DESC_F_WRITE	2u

#define QUEUE_RX		0u
#define QUEUE_TX		1u
#define QUEUE_SIZE		8u
#define BUF_SIZE		1536u

/* Modern virtio-mmio (version 2) — Linux uapi offsets */
#define VIRTIO_MMIO_MAGIC		0x000
#define VIRTIO_MMIO_VERSION		0x004
#define VIRTIO_MMIO_DEVICE_ID		0x008
#define VIRTIO_MMIO_DEVICE_FEATURES	0x010
#define VIRTIO_MMIO_DEVICE_FEATURES_SEL	0x014
#define VIRTIO_MMIO_DRIVER_FEATURES	0x020
#define VIRTIO_MMIO_DRIVER_FEATURES_SEL	0x024
#define VIRTIO_MMIO_QUEUE_SEL		0x030
#define VIRTIO_MMIO_QUEUE_NUM_MAX	0x034
#define VIRTIO_MMIO_QUEUE_NUM		0x038
#define VIRTIO_MMIO_QUEUE_READY		0x044
#define VIRTIO_MMIO_QUEUE_NOTIFY	0x050
#define VIRTIO_MMIO_INTERRUPT_STATUS	0x060
#define VIRTIO_MMIO_INTERRUPT_ACK	0x064
#define VIRTIO_MMIO_STATUS		0x070
#define VIRTIO_MMIO_QUEUE_DESC_LOW	0x080
#define VIRTIO_MMIO_QUEUE_DESC_HIGH	0x084
#define VIRTIO_MMIO_QUEUE_AVAIL_LOW	0x090
#define VIRTIO_MMIO_QUEUE_AVAIL_HIGH	0x094
#define VIRTIO_MMIO_QUEUE_USED_LOW	0x0a0
#define VIRTIO_MMIO_QUEUE_USED_HIGH	0x0a4
#define VIRTIO_MMIO_CONFIG		0x100

struct virtq_desc {
  uint64_t addr;
  uint32_t len;
  uint16_t flags;
  uint16_t next;
} __attribute__((packed));

struct virtq_avail {
  uint16_t flags;
  uint16_t idx;
  uint16_t ring[QUEUE_SIZE];
} __attribute__((packed));

struct virtq_used_elem {
  uint32_t id;
  uint32_t len;
} __attribute__((packed));

struct virtq_used {
  uint16_t flags;
  uint16_t idx;
  struct virtq_used_elem ring[QUEUE_SIZE];
} __attribute__((packed));

struct virtio_net_hdr {
  uint8_t flags;
  uint8_t gso_type;
  uint16_t hdr_len;
  uint16_t gso_size;
  uint16_t csum_start;
  uint16_t csum_offset;
  uint16_t num_buffers; /* present with VIRTIO_F_VERSION_1 */
} __attribute__((packed));

#define REG(off) (*(volatile uint32_t *)(VIRTIO_MMIO_BASE + (off)))

static struct virtq_desc rx_desc[QUEUE_SIZE] __attribute__((aligned(16)));
static struct virtq_avail rx_avail __attribute__((aligned(2)));
static struct virtq_used rx_used __attribute__((aligned(4)));
static uint8_t rx_buf[QUEUE_SIZE][BUF_SIZE] __attribute__((aligned(16)));

static struct virtq_desc tx_desc[QUEUE_SIZE] __attribute__((aligned(16)));
static struct virtq_avail tx_avail __attribute__((aligned(2)));
static struct virtq_used tx_used __attribute__((aligned(4)));
static uint8_t tx_buf[QUEUE_SIZE][BUF_SIZE] __attribute__((aligned(16)));

static uint16_t rx_last_used;
static uint16_t tx_last_used;
static uint8_t tx_free_count;
static uint8_t tx_free_list[QUEUE_SIZE];

static struct netdev g_vdev;
static int g_ready;

static uint64_t virtio_get_features(void)
{
  uint64_t f;

  REG(VIRTIO_MMIO_DEVICE_FEATURES_SEL) = 0;
  f = REG(VIRTIO_MMIO_DEVICE_FEATURES);
  REG(VIRTIO_MMIO_DEVICE_FEATURES_SEL) = 1;
  f |= ((uint64_t)REG(VIRTIO_MMIO_DEVICE_FEATURES)) << 32;
  return f;
}

static void virtio_set_features(uint64_t f)
{
  REG(VIRTIO_MMIO_DRIVER_FEATURES_SEL) = 0;
  REG(VIRTIO_MMIO_DRIVER_FEATURES) = (uint32_t)f;
  REG(VIRTIO_MMIO_DRIVER_FEATURES_SEL) = 1;
  REG(VIRTIO_MMIO_DRIVER_FEATURES) = (uint32_t)(f >> 32);
}

static int virtq_setup(uint32_t qsel, struct virtq_desc *desc,
                       struct virtq_avail *avail, struct virtq_used *used)
{
  uint32_t qmax;

  REG(VIRTIO_MMIO_QUEUE_SEL) = qsel;
  qmax = REG(VIRTIO_MMIO_QUEUE_NUM_MAX);
  if (qmax == 0 || QUEUE_SIZE > qmax)
    return -1;
  REG(VIRTIO_MMIO_QUEUE_NUM) = QUEUE_SIZE;
  REG(VIRTIO_MMIO_QUEUE_READY) = 0;
  REG(VIRTIO_MMIO_QUEUE_DESC_LOW) = (uint32_t)(uintptr_t)desc;
  REG(VIRTIO_MMIO_QUEUE_DESC_HIGH) = (uint32_t)((uint64_t)(uintptr_t)desc >> 32);
  REG(VIRTIO_MMIO_QUEUE_AVAIL_LOW) = (uint32_t)(uintptr_t)avail;
  REG(VIRTIO_MMIO_QUEUE_AVAIL_HIGH) = (uint32_t)((uint64_t)(uintptr_t)avail >> 32);
  REG(VIRTIO_MMIO_QUEUE_USED_LOW) = (uint32_t)(uintptr_t)used;
  REG(VIRTIO_MMIO_QUEUE_USED_HIGH) = (uint32_t)((uint64_t)(uintptr_t)used >> 32);
  __asm__ volatile("dmb sy" ::: "memory");
  REG(VIRTIO_MMIO_QUEUE_READY) = 1;
  if (REG(VIRTIO_MMIO_QUEUE_READY) != 1)
    return -1;
  return 0;
}

static void rx_fill(uint16_t idx)
{
  rx_desc[idx].addr = (uint64_t)(uintptr_t)rx_buf[idx];
  rx_desc[idx].len = BUF_SIZE;
  rx_desc[idx].flags = VRING_DESC_F_WRITE;
  rx_desc[idx].next = 0;
  rx_avail.ring[rx_avail.idx % QUEUE_SIZE] = idx;
  __asm__ volatile("dmb sy" ::: "memory");
  rx_avail.idx++;
  __asm__ volatile("dmb sy" ::: "memory");
  REG(VIRTIO_MMIO_QUEUE_NOTIFY) = QUEUE_RX;
}

#define VIRTIO_FLG_RX   (1u << 0)

static uint64_t vnet_tsk_stack[VIRTIO_NET_TSK_STACK_SIZE / sizeof(uint64_t)];

static void vnet_process(void)
{
  while (rx_last_used != rx_used.idx) {
    struct virtq_used_elem *e = &rx_used.ring[rx_last_used % QUEUE_SIZE];
    uint16_t di = (uint16_t)e->id;
    uint32_t len = e->len;
    uint8_t *p = rx_buf[di];

    if (len > sizeof(struct virtio_net_hdr) && g_vdev.rx) {
      g_vdev.rx(&g_vdev, p + sizeof(struct virtio_net_hdr),
                len - sizeof(struct virtio_net_hdr), g_vdev.rx_arg);
    }
    rx_last_used++;
    rx_fill(di);
  }

  while (tx_last_used != tx_used.idx) {
    uint16_t di = (uint16_t)tx_used.ring[tx_last_used % QUEUE_SIZE].id;
    lock_state_t ls;
    begin_critical_section(ls);
    if (tx_free_count < QUEUE_SIZE)
      tx_free_list[tx_free_count++] = (uint8_t)di;
    end_critical_section(ls);
    tx_last_used++;
  }
}

static void virtio_net_irq(void)
{
  uint32_t st = REG(VIRTIO_MMIO_INTERRUPT_STATUS);

  REG(VIRTIO_MMIO_INTERRUPT_ACK) = st;
  iset_flg(VIRTIO_FLG_ID, VIRTIO_FLG_RX);
}

static void vnet_task(void *exinf)
{
  FLGPTN ptn;

  (void)exinf;
  for (;;) {
    twai_flg(VIRTIO_FLG_ID, VIRTIO_FLG_RX, TWF_ORW, &ptn, TMO_FEVR);
    clr_flg(VIRTIO_FLG_ID, ~VIRTIO_FLG_RX);
    vnet_process();
  }
}

static int vnet_start_task(void)
{
  T_CFLG pk_cflg;
  T_CTSK pk;

  pk_cflg.flgatr = TA_TPRI | TA_WMUL;
  pk_cflg.iflgptn = 0;
  if (cre_flg(VIRTIO_FLG_ID, &pk_cflg) != E_OK)
    return -1;

  pk.tskatr = TA_HLNG | TA_NAME;
  pk.exinf = 0;
  pk.task = (FP)vnet_task;
  pk.itskpri = VIRTIO_NET_TASK_PRIO;
  pk.stksz = VIRTIO_NET_TSK_STACK_SIZE;
  pk.stk = vnet_tsk_stack;
  pk.name = "virtio-net";
  if (cre_tsk(VIRTIO_NET_TASK_ID, &pk) != E_OK)
    return -1;
  return act_tsk(VIRTIO_NET_TASK_ID) == E_OK ? 0 : -1;
}

static int vnet_init(struct netdev *dev)
{
  uint64_t feat;
  uint32_t i;

  (void)dev;
  if (REG(VIRTIO_MMIO_MAGIC) != VIRTIO_MAGIC ||
      REG(VIRTIO_MMIO_VERSION) != VIRTIO_VERSION)
    return -1;
  if (REG(VIRTIO_MMIO_DEVICE_ID) != VIRTIO_DEV_NET)
    return -1;

  REG(VIRTIO_MMIO_STATUS) = 0;
  REG(VIRTIO_MMIO_STATUS) = VIRTIO_STATUS_ACK;
  REG(VIRTIO_MMIO_STATUS) = VIRTIO_STATUS_ACK | VIRTIO_STATUS_DRIVER;

  feat = virtio_get_features();
  feat &= VIRTIO_F_VERSION_1 | VIRTIO_NET_F_MAC | VIRTIO_NET_F_STATUS;
  virtio_set_features(feat);
  /* FEATURES_OK must be set via status write after DriverFeatures (QEMU applies then) */
  REG(VIRTIO_MMIO_STATUS) = VIRTIO_STATUS_ACK | VIRTIO_STATUS_DRIVER |
                            VIRTIO_STATUS_FEATURES_OK;
  if (!(REG(VIRTIO_MMIO_STATUS) & VIRTIO_STATUS_FEATURES_OK)) {
    REG(VIRTIO_MMIO_STATUS) = VIRTIO_STATUS_FAILED;
    return -1;
  }

  if (feat & VIRTIO_NET_F_MAC) {
    for (i = 0; i < 6; i++)
      g_vdev.mac[i] = *(volatile uint8_t *)(VIRTIO_MMIO_BASE + VIRTIO_MMIO_CONFIG + i);
  } else {
    g_vdev.mac[0] = 0x52;
    g_vdev.mac[1] = 0x54;
    g_vdev.mac[2] = 0x00;
    g_vdev.mac[3] = 0x12;
    g_vdev.mac[4] = 0x34;
    g_vdev.mac[5] = 0x56;
  }

  memset(&rx_avail, 0, sizeof(rx_avail));
  memset(&rx_used, 0, sizeof(rx_used));
  memset(&tx_avail, 0, sizeof(tx_avail));
  memset(&tx_used, 0, sizeof(tx_used));
  rx_last_used = 0;
  tx_last_used = 0;
  tx_free_count = QUEUE_SIZE;
  for (i = 0; i < QUEUE_SIZE; i++)
    tx_free_list[i] = (uint8_t)i;

  if (virtq_setup(QUEUE_RX, rx_desc, &rx_avail, &rx_used) != 0)
    return -1;
  for (i = 0; i < QUEUE_SIZE; i++)
    rx_fill((uint16_t)i);

  if (virtq_setup(QUEUE_TX, tx_desc, &tx_avail, &tx_used) != 0)
    return -1;

  install_handler(virtio_net_irq, VIRTIO_NET_VEC_NO, VIRTIO_NET_IRQ_PRIO);
  gic_enable_irq(VIRTIO_NET_IRQ, VIRTIO_NET_IRQ_PRIO);

  if (vnet_start_task() != 0)
    return -1;

  REG(VIRTIO_MMIO_STATUS) = VIRTIO_STATUS_ACK | VIRTIO_STATUS_DRIVER |
                            VIRTIO_STATUS_FEATURES_OK | VIRTIO_STATUS_DRIVER_OK;
  /* Kick RX after DRIVER_OK — notifies before that may be ignored */
  REG(VIRTIO_MMIO_QUEUE_NOTIFY) = QUEUE_RX;
  g_ready = 1;
  return 0;
}

static int vnet_start(struct netdev *dev)
{
  (void)dev;
  return g_ready ? 0 : -1;
}

static int vnet_tx(struct netdev *dev, const void *data, size_t len)
{
  uint8_t idx;
  struct virtio_net_hdr *hdr;
  lock_state_t ls;

  (void)dev;
  if (!g_ready || !data || len == 0 ||
      len + sizeof(*hdr) > BUF_SIZE)
    return -1;

  /* Reclaim completed TX before taking a descriptor */
  while (tx_last_used != tx_used.idx) {
    uint16_t di = (uint16_t)tx_used.ring[tx_last_used % QUEUE_SIZE].id;
    begin_critical_section(ls);
    if (tx_free_count < QUEUE_SIZE)
      tx_free_list[tx_free_count++] = (uint8_t)di;
    end_critical_section(ls);
    tx_last_used++;
  }

  begin_critical_section(ls);
  if (tx_free_count == 0) {
    end_critical_section(ls);
    return -1;
  }
  idx = tx_free_list[--tx_free_count];
  end_critical_section(ls);

  hdr = (struct virtio_net_hdr *)tx_buf[idx];
  memset(hdr, 0, sizeof(*hdr));
  memcpy(tx_buf[idx] + sizeof(*hdr), data, len);

  tx_desc[idx].addr = (uint64_t)(uintptr_t)tx_buf[idx];
  tx_desc[idx].len = (uint32_t)(sizeof(*hdr) + len);
  tx_desc[idx].flags = 0;
  tx_desc[idx].next = 0;

  begin_critical_section(ls);
  tx_avail.ring[tx_avail.idx % QUEUE_SIZE] = idx;
  __asm__ volatile("dmb sy" ::: "memory");
  tx_avail.idx++;
  __asm__ volatile("dmb sy" ::: "memory");
  REG(VIRTIO_MMIO_QUEUE_NOTIFY) = QUEUE_TX;
  end_critical_section(ls);
  return (int)len;
}

static const struct netdev_ops g_vnet_ops = {
  .init = vnet_init,
  .start = vnet_start,
  .tx = vnet_tx,
};

int virtio_net_init(void)
{
  g_vdev.name = "virtio0";
  g_vdev.ops = &g_vnet_ops;
  g_vdev.priv = 0;
  if (netdev_register(&g_vdev) != 0)
    return -1;
  if (netdev_init(&g_vdev) != 0)
    return -1;
  return netdev_start(&g_vdev);
}

struct netdev *virtio_net_dev(void)
{
  return g_ready ? &g_vdev : 0;
}
