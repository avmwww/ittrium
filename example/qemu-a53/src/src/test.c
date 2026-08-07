/**
 * qemu-a53 init (CFG_USE_* in kernel_config.h)
 */
#include "ittrium.h"
#include "target.h"

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
#endif

uint64_t init_tsk_stack[INIT_TASK_STACK_SIZE / sizeof(uint64_t)];
#if CFG_USE_SHELL
uint64_t shell_tsk_stack[SHELL_TSK_STACK_SIZE / sizeof(uint64_t)];
#endif

#if CFG_USE_SHELL
static void start_shell(void)
{
  T_CTSK pk;

  shell_set_io(uart_putc, uart_getc);

  pk.tskatr = TA_HLNG;
  pk.exinf = 0;
  pk.task = (FP)shell_task;
  pk.itskpri = SHELL_TASK_PRIO;
  pk.stksz = SHELL_TSK_STACK_SIZE;
  pk.stk = shell_tsk_stack;

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
    uart_puts("lfs: smoke open-w failed\n");
    return;
  }
  n = vfs_write(fd, "ok\n", 3);
  vfs_close(fd);
  if (n != 3) {
    uart_puts("lfs: smoke write failed\n");
    return;
  }

  fd = vfs_open("/data/smoke.txt", VFS_O_RDONLY);
  if (fd < 0) {
    uart_puts("lfs: smoke open-r failed\n");
    return;
  }
  n = vfs_read(fd, buf, sizeof(buf) - 1);
  vfs_close(fd);
  if (n != 3 || buf[0] != 'o' || buf[1] != 'k') {
    uart_puts("lfs: smoke read failed\n");
    return;
  }
  uart_puts("lfs: smoke ok\n");
}
#endif

void init_tsk(void *exinf)
{
  (void)exinf;

  uart_init();
  uart_puts("ittrium cortex-a53 qemu\n");

#if CFG_USE_VFS
  uart_puts("vfs...\n");
  vfs_init();
#endif
#if CFG_USE_ROMFS
  uart_puts("romfs...\n");
  romfs_mount("/", NULL, 0);
#endif
#if CFG_USE_PROCFS
  uart_puts("proc...\n");
  procfs_mount("/proc");
#endif
#if CFG_USE_SYSFS
  uart_puts("sys...\n");
  sysfs_mount("/sys");
#endif
#if CFG_USE_LFS
  uart_puts("data...\n");
  if (lfs_port_mount("/data") != 0)
    uart_puts("lfs: mount failed\n");
  else {
    uart_puts("lfs: /data ready\n");
    lfs_smoke();
  }
#elif CFG_USE_RAMFS
  uart_puts("data...\n");
  if (ramfs_mount("/data") != 0)
    uart_puts("ramfs: mount failed\n");
  else
    uart_puts("ramfs: /data ready\n");
#endif

#if CFG_USE_LWIP
  uart_puts("lwip: init...\n");
  net_init();
  uart_puts("lwip: loopback + echo :7\n");
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
