#ifndef _KERNEL_CONFIG_H_
#define _KERNEL_CONFIG_H_

#include <stdint.h>

#define TIC_NUME      1
#define TIC_DENO   1000

#define CFG_USE_VFS      1
#define CFG_USE_ROMFS    1
#define CFG_USE_RAMFS    0
#define CFG_USE_LFS      0
#define CFG_USE_PROCFS   1
#define CFG_USE_SYSFS    0
#define CFG_USE_SHELL    1
#define CFG_USE_ELF      0
#define CFG_USE_TSKNAME  1
#define CFG_USE_LWIP     0

enum {
  INIT_TASK_ID = TMIN_TSKID,
#if CFG_USE_SHELL
  SHELL_TASK_ID,
#endif
  MAX_TASK_ID
};
#define TMAX_TSKID    (MAX_TASK_ID - 1)

#define HIGH_PRIO             TMIN_TPRI
#define LOW_PRIO              (TMIN_TPRI + 10)
#define TMAX_TPRI             LOW_PRIO

#define INIT_TASK_STACK_SIZE   2048
#define IDLE_TSK_STACK_SIZE    512
#if CFG_USE_SHELL
#define SHELL_TSK_STACK_SIZE   4096
#define SHELL_TASK_PRIO       (HIGH_PRIO + 4)
#endif

void init_tsk(void *exinf);
extern uint32_t init_tsk_stack[];
#define INIT_TASK_ATTR         TA_HLNG
#define INIT_TASK_EXINF        (0)
#define INIT_TASK              init_tsk
#define INIT_TASK_PRI          HIGH_PRIO
#define INIT_TASK_STACK        init_tsk_stack
#define INIT_TASK_NAME         "init"

#define TBIT_FLGPTN              16
enum {
  UART_FLG_ID = TMIN_FLGID,
  MAX_FLAG_ID
};
#define TMAX_FLGID    (MAX_FLAG_ID - 1)

enum {
  DEMO_MBX_ID = TMIN_MBXID,
  MAX_MBX_ID
};
#define TMAX_MBXID    (MAX_MBX_ID - 1)

#define TMAX_MAXSEM              16
#define TICKER                    1
#define TMAX_ACTCNT              15

#endif
