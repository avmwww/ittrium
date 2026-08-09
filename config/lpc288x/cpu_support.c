#include "ittrium.h"
#include "../kernel/task.h"
#include "../cpu_generic.c"


struct ivt_t {
  FP  func;
  UW  prio;
} static int_vector_table[32];

/* Matches dispatch_r: ldmfd {r4-r11,lr}; mov pc,lr */
extern void dispatch_r(void);

void make_task_context(TCB *tcb)
{
  UW *pstk;

  pstk = (UW *)((UW)tcb->stk & ~7UL);
  *--pstk = (UW)tcb->task; /* LR -> task entry */
  *--pstk = 0x11111111UL;
  *--pstk = 0x10101010UL;
  *--pstk = 0x09090909UL;
  *--pstk = 0x08080808UL;
  *--pstk = 0x07070707UL;
  *--pstk = 0x06060606UL;
  *--pstk = 0x05050505UL;
  *--pstk = 0x04040404UL;

  tcb->tskctxb.sp = pstk;
  tcb->tskctxb.pc = (VP)dispatch_r;
}

void interrupt_handler(INHNO vector)
{
  if (int_vector_table[vector].func) {
    cpu_unlock();
    int_vector_table[vector].func();
  }
}

void _install_handler(FP handler, INHNO vec_no, UB prio)
{
  if (vec_no < 32) {
    BEGIN_CRITICAL_SECTION;
    int_vector_table[vec_no].func = handler;
    int_vector_table[vec_no].prio = prio;
    END_CRITICAL_SECTION;
  }
}

void _int_init(void)
{
  int i;

  int_nesting = 0;
  for (i = 0;  i < 32; i++) {
    int_vector_table[i].func = (FP)0;
    int_vector_table[i].prio = 0;
  }
}

void start_hw_timer(void)
{
  timer_nesting = 0;
  INIT_SYSTEM_TICKS();

  install_handler(timer_handler, TICKER_VEC_NO, TICKER_PRIO);
  ENABLE_TICKER_INT();

  INT_VECTOR0 = IRQ_TABLE_BASE & MASK_INDEX;

  T0LOAD =  TIMER_1MS;
  T0VALUE = TIMER_1MS;
  T0CLR = 0;
  T0CTRL=0xC8;
}

void terminate_hw_timer(void)
{
  DISABLE_TICKER_INT();
  int_vector_table[TICKER_VEC_NO].func = (FP)0;
  int_vector_table[TICKER_VEC_NO].prio = 0;
}

void low_level_init(void)
{
  _int_init();
}

void install_handler(FP handler, INHNO vec_no, UB prio)
{
  _install_handler(handler, vec_no, prio);
}
