#ifndef _KERNEL_CONFIG_H_
#define _KERNEL_CONFIG_H_

#include <stdint.h>

/* 1 tick = 1 ms */
#define TIC_NUME      1
#define TIC_DENO   1000

//==============================================================================
// Tasks
//==============================================================================
enum {
  INIT_TASK_ID = TMIN_TSKID,
  SHELL_TASK_ID,
  /* lwIP thread pool (must be contiguous for LWIP_TASK_ID_BASE) */
  TCPIP_TASK_ID,
  LWIP_TASK_2_ID,
  LWIP_TASK_3_ID,
  LWIP_TASK_4_ID,
  ELF_TASK_ID,
  MAX_TASK_ID
};
#define TMAX_TSKID    (MAX_TASK_ID - 1)

#define LWIP_TASK_ID_BASE   TCPIP_TASK_ID
#define LWIP_THREAD_COUNT   4

#define INIT_TASK_STACK_SIZE   8192
#define SHELL_TSK_STACK_SIZE   8192
#define ELF_TSK_STACK_SIZE     8192
#define IDLE_TSK_STACK_SIZE    2048

#define HIGH_PRIO             TMIN_TPRI
#define LOW_PRIO              (TMIN_TPRI + 12)
#define SHELL_TASK_PRIO       (HIGH_PRIO + 4)
#define TMAX_TPRI             LOW_PRIO

void init_tsk(void *exinf);
extern uint64_t init_tsk_stack[];
#define INIT_TASK_ATTR         TA_HLNG
#define INIT_TASK_EXINF        (0)
#define INIT_TASK              init_tsk
#define INIT_TASK_PRI          HIGH_PRIO
#define INIT_TASK_STACK        init_tsk_stack

//==============================================================================
// Eventflags
//==============================================================================
#define TBIT_FLGPTN              16
enum {
  UART_FLG_ID = TMIN_FLGID,
  MAX_FLAG_ID
};
#define TMAX_FLGID    (MAX_FLAG_ID - 1)

//==============================================================================
// Semaphores (lwIP pool)
//==============================================================================
#define TMAX_MAXSEM              32

enum {
  LWIP_SEM_FIRST = TMIN_SEMID,
  /* 32 IDs for lwIP sys_arch */
  LWIP_SEM_LAST = LWIP_SEM_FIRST + 31,
  MAX_SEM_ID
};
#define TMAX_SEMID       (MAX_SEM_ID - 1)
#define LWIP_SEM_ID_BASE LWIP_SEM_FIRST
#define LWIP_SEM_COUNT   32

#define TICKER                    1
#define TMAX_ACTCNT              15

#endif /* _KERNEL_CONFIG_H_ */
