#ifndef _KERNEL_CONFIG_H_
#define _KERNEL_CONFIG_H_

#define TIC_NUME      1
#define TIC_DENO   1000

enum {
  INIT_TASK_ID = TMIN_TSKID,
  TEST_TASK_ID,
  MAX_TASK_ID
};
#define TMAX_TSKID    (MAX_TASK_ID - 1)

#define HIGH_PRIO             TMIN_TPRI
#define LOW_PRIO              (TMIN_TPRI + 10)
#define TMAX_TPRI             LOW_PRIO

#define TEST_TSK_STACK_SIZE   1024
#define INIT_TASK_STACK_SIZE  1024

void init_tsk(void *exinf);
extern unsigned short init_tsk_stack[];

#define INIT_TASK_ATTR         TA_HLNG
#define INIT_TASK_EXINF        (0)
#define INIT_TASK              init_tsk
#define INIT_TASK_PRI          HIGH_PRIO
#define INIT_TASK_STACK        init_tsk_stack


#define TEST_TASK_PRIO         (HIGH_PRIO + 6)

#define TBIT_FLGPTN              16
enum {
  TEST_FLG_ID = TMIN_FLGID,
  MAX_FLAG_ID
};
#define TMAX_FLGID    (MAX_FLAG_ID - 1)

#define TICKER                    1
#define TMAX_ACTCNT              15

/* Board must provide real timer enable; stubs compile for IAR bring-up */
#ifndef TICKER_VEC_NO
#define TICKER_VEC_NO        0
#endif
#ifndef ENABLE_TICKER_INT
#define ENABLE_TICKER_INT()  ((void)0)
#define DISABLE_TICKER_INT() ((void)0)
#endif


#endif /* _KERNEL_CONFIG_H_ */
