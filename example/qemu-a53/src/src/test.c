/**
 * qemu-a53 init (CFG_USE_* in kernel_config.h)
 */
#include "ittrium.h"
#include "target.h"
#include "drv/console.h"

#if CFG_USE_SHELL
#include "shell.h"
#endif
#if CFG_USE_VFS
#include "vfs.h"
#endif
#if CFG_USE_ROMFS
#include "romfs.h"
#endif
#if CFG_USE_LFS
#include "lfs_port.h"
#endif
#if CFG_USE_RAMFS
#include "ramfs.h"
#endif
#if CFG_USE_PROCFS
#include "procfs.h"
#endif
#if CFG_USE_SYSFS
#include "sysfs.h"
#endif
#if CFG_USE_LWIP
#include "net_init.h"
#include "netif_netdev.h"
#include "lwip/ip4_addr.h"
#endif
#if CFG_USE_ELF && CFG_USE_ROMFS
/* Embedded by example/qemu-a53/app → hello_elf_bin.o */
extern const unsigned char _binary_hello_elf_start[];
extern const unsigned char _binary_hello_elf_end[];
#endif

uint64_t init_tsk_stack[INIT_TASK_STACK_SIZE / sizeof(uint64_t)];
#if CFG_USE_SHELL
uint64_t shell_tsk_stack[SHELL_TSK_STACK_SIZE / sizeof(uint64_t)];
#endif

#if CFG_USE_SHELL
static void start_shell(void)
{
  T_CTSK pk;

  shell_set_io(console_putc, console_getc);

  pk.tskatr = TA_HLNG | TA_NAME;
  pk.exinf = 0;
  pk.task = (FP)shell_task;
  pk.itskpri = SHELL_TASK_PRIO;
  pk.stksz = SHELL_TSK_STACK_SIZE;
  pk.stk = shell_tsk_stack;
  pk.name = "shell";

  cre_tsk(SHELL_TASK_ID, &pk);
  act_tsk(SHELL_TASK_ID);
}
#endif

#if CFG_USE_LFS
static void lfs_smoke(void)
{
  int fd, n;
  char buf[16];

  fd = vfs_open("/data/smoke.txt", VFS_O_WRONLY | VFS_O_CREAT | VFS_O_TRUNC);
  if (fd < 0) {
    console_puts("lfs: smoke open-w failed\n");
    return;
  }
  n = vfs_write(fd, "ok\n", 3);
  vfs_close(fd);
  if (n != 3) {
    console_puts("lfs: smoke write failed\n");
    return;
  }

  fd = vfs_open("/data/smoke.txt", VFS_O_RDONLY);
  if (fd < 0) {
    console_puts("lfs: smoke open-r failed\n");
    return;
  }
  n = vfs_read(fd, buf, sizeof(buf) - 1);
  vfs_close(fd);
  if (n != 3 || buf[0] != 'o' || buf[1] != 'k') {
    console_puts("lfs: smoke read failed\n");
    return;
  }
  console_puts("lfs: smoke ok\n");
}
#endif

void init_tsk(void *exinf)
{
  (void)exinf;

  uart_init();
  console_puts("ittrium cortex-a53 qemu\n");

#if CFG_USE_VFS
  console_puts("vfs...\n");
  vfs_init();
#endif
#if CFG_USE_ROMFS
  console_puts("romfs...\n");
#if CFG_USE_ELF
  {
    size_t n = (size_t)(_binary_hello_elf_end - _binary_hello_elf_start);
    if (romfs_add_builtin("hello.elf", _binary_hello_elf_start, n) != 0)
      console_puts("romfs: hello.elf add failed\n");
  }
#endif
  romfs_mount("/", NULL, 0);
#endif
#if CFG_USE_PROCFS
  console_puts("proc...\n");
  procfs_mount("/proc");
#endif
#if CFG_USE_SYSFS
  console_puts("sys...\n");
  sysfs_mount("/sys");
#endif
#if CFG_USE_LFS
  console_puts("data...\n");
  if (lfs_port_mount("/data") != 0)
    console_puts("lfs: mount failed\n");
  else {
    console_puts("lfs: /data ready\n");
    lfs_smoke();
  }
#elif CFG_USE_RAMFS
  console_puts("data...\n");
  if (ramfs_mount("/data") != 0)
    console_puts("ramfs: mount failed\n");
  else
    console_puts("ramfs: /data ready\n");
#endif

#if CFG_USE_LWIP
  console_puts("virtio-net...\n");
  if (virtio_net_init() != 0)
    console_puts("virtio-net: init failed\n");
  else {
    static struct netif eth;
    ip4_addr_t ip, mask, gw;

    console_puts("lwip: init...\n");
    net_init();
    /* QEMU user-net: guest 10.0.2.15, gateway 10.0.2.2 */
    IP4_ADDR(&ip, 10, 0, 2, 15);
    IP4_ADDR(&mask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 10, 0, 2, 2);
    if (netif_netdev_add(&eth, virtio_net_dev(), &ip, &mask, &gw) != ERR_OK)
      console_puts("lwip: eth add failed\n");
    else {
      netif_set_default(&eth);
      console_puts("lwip: eth 10.0.2.15 echo :7 (host :10007)\n");
    }
  }
#endif

#if CFG_USE_SHELL
  start_shell();
#endif
  chg_pri(TSK_SELF, LOW_PRIO);

  for (;;)
    dly_tsk(10000);
}

void _low_level_init(void)
{
}
