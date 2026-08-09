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
extern unsigned long init_tsk_stack[];

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

/* LPC288x / ARM7 timer + VIC stubs (override with real chip headers as needed) */
#ifndef TIMER_1MS
#define TIMER_1MS           60000u
#endif
#ifndef IRQ_TABLE_BASE
#define IRQ_TABLE_BASE      0x00000000u
#endif
#ifndef MASK_INDEX
#define MASK_INDEX          0xFFFFF000u
#endif
#ifndef T0LOAD
#define T0LOAD   (*(volatile unsigned long *)0x80020000)
#define T0VALUE  (*(volatile unsigned long *)0x80020004)
#define T0CTRL   (*(volatile unsigned long *)0x80020008)
#define T0CLR    (*(volatile unsigned long *)0x8002000C)
#define INT_REQ5 (*(volatile unsigned long *)0x80300014)
#define INT_VECTOR0 (*(volatile unsigned long *)0x80300000)
#endif

/* arm7tdmi port expects BSP helpers */
int install_IRQ(int vec, int prio, void (*handler)(void));
void init_interrupt(void);


#endif /* _KERNEL_CONFIG_H_ */
