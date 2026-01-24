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
  IDLE_TASK_ID             ,
  USB_TASK_ID              ,
  MAX_TASK_ID
};
#define TMAX_TSKID    (MAX_TASK_ID - 1)
//==============================================================================
// Stacks
//==============================================================================
#define IDLE_TSK_STACK_SIZE     256
//==============================================================================
// Priority
//==============================================================================
#define HIGH_PRIO             TMIN_TPRI
#define LOW_PRIO              (TMIN_TPRI + 7)

// Maximum task priority
#define TMAX_TPRI             (LOW_PRIO + 1)

enum {
	IDLE_TASK_PRIO = LOW_PRIO,
	USB_TASK_PRIO = HIGH_PRIO + 1,
};
//==============================================================================
// Initialize task
//==============================================================================
void init_tsk(void *exinf);
extern UW init_tsk_stack[];

#define INIT_TASK_ATTR         TA_HLNG
#define INIT_TASK_EXINF        (0)
#define INIT_TASK              init_tsk
#define INIT_TASK_PRI          HIGH_PRIO
#define INIT_TASK_STACK_SIZE   1024
#define INIT_TASK_STACK        init_tsk_stack
//

#define IDLE_TASK_ATTR         TA_HLNG
#define IDLE_TASK_EXINF        (0)
//==============================================================================
// Eventflags
//==============================================================================
// The number of bits in an eventflag
#define TBIT_FLGPTN              16
enum{
  BUTTON_FLG_ID  = TMIN_FLGID,
  USB_FLAG_ID,
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
// vector numbers
enum {
    USB_IRQ_VEC_NO = VEC_NO_LAST,
};

#define USB_IRQ_PRIO            3

#endif // _KERNEL_CONFIG_H_


