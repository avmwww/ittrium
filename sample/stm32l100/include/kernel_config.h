#ifndef _KERNEL_CONFIG_H_
#define _KERNEL_CONFIG_H_

// 1 tic per 1 ms
#define TIC_NUME      1
#define TIC_DENO   1000
//==============================================================================
// Task
//==============================================================================
enum{
  // SYSTEM TASKS ID
  INIT_TASK_ID = TMIN_TSKID,
  TEST_TASK_ID             ,
  MAX_TASK_ID
};
#define TMAX_TSKID    (MAX_TASK_ID - 1)
//==============================================================================
// Stacks
//==============================================================================
#define TEST_TSK_STACK_SIZE   256
//==============================================================================
// Priority
//==============================================================================
#define HIGH_PRIO             TMIN_TPRI
#define LOW_PRIO              (TMIN_TPRI + 10)

#define TEST_TASK_PRIO         (HIGH_PRIO + 6)
// Maximum task priority
#define TMAX_TPRI             LOW_PRIO
//==============================================================================
// Initialize task
//==============================================================================
void init_tsk(void *exinf);
extern short init_tsk_stack[];
extern short init_tsk_sys_stack[];
#define INIT_TASK_ATTR         TA_HLNG
#define INIT_TASK_EXINF        (0)
#define INIT_TASK              init_tsk
#define INIT_TASK_PRI          HIGH_PRIO
#define INIT_TASK_STACK_SIZE   512
#define INIT_TASK_STACK        init_tsk_stack
#define INIT_TASK_SYS_STACK    init_tsk_sys_stack
// IDLE task
#define IDLE_TSK_STACK_SIZE     256
//==============================================================================
// Eventflags
//==============================================================================
// The number of bits in an eventflag
#define TBIT_FLGPTN              16
enum{
  TEST_FLG_ID = TMIN_FLGID,
  MAX_FLAG_ID
};
#define TMAX_FLGID    (MAX_FLAG_ID - 1)
//==============================================================================
// Semaphores
//==============================================================================
// Maximum value of the maximum definable semaphore resource count
#define TMAX_MAXSEM              16
//enum{
//  TEST_SEM_ID = TMIN_SEMID,
//  MAX_SEM_ID
//};
//#define TMAX_SEMID       (MAX_SEM_ID - 1)

#define TICKER                    1

#define TMAX_ACTCNT              15 // Maximum activation request count

// Mailbox
//#define NUMBER_OF_MAILBOXES      16
//------------------------------------------------------------------------------

#endif // _KERNEL_CONFIG_H_


