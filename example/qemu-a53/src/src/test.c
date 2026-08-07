/**
 * qemu-a53 platform bring-up: VFS, proc/sys, littlefs, lwIP, shell
 */
#include "ittrium.h"
#include "shell.h"
#include "vfs.h"
#include "romfs.h"
#include "ramfs.h"
#include "procfs.h"
#include "sysfs.h"
#include "net_init.h"
#include "target.h"

uint64_t init_tsk_stack[INIT_TASK_STACK_SIZE / sizeof(uint64_t)];
uint64_t shell_tsk_stack[SHELL_TSK_STACK_SIZE / sizeof(uint64_t)];

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

void init_tsk(void *exinf)
{
  (void)exinf;

  uart_init();
  uart_puts("ittrium cortex-a53 qemu\n");

  uart_puts("vfs...\n");
  vfs_init();
  uart_puts("romfs...\n");
  romfs_mount("/", NULL, 0);
  uart_puts("proc...\n");
  procfs_mount("/proc");
  uart_puts("sys...\n");
  sysfs_mount("/sys");
  uart_puts("data...\n");
  if (ramfs_mount("/data") != 0)
    uart_puts("ramfs: mount failed\n");
  else
    uart_puts("ramfs: /data ready\n");
  /* littlefs: services/vfs/lfs_port.c — TBD (hangs in lfs_mount on this target) */

  uart_puts("lwip: init...\n");
  net_init();
  uart_puts("lwip: loopback + echo :7\n");

  start_shell();
  chg_pri(TSK_SELF, LOW_PRIO);

  for (;;)
    dly_tsk(10000);
}

void _low_level_init(void)
{
}
