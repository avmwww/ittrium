/**
 * @brief Generic ARM7 port
 */
#include "ittrium.h"
#include "../kernel/task.h"
#include "../cpu_generic.c"

#define  ARM_MODE_ARM           0x00000000
#define  ARM_MODE_THUMB         0x00000020

#define  ARM_SYS_MODE_THUMB    (0x0000001FL + ARM_MODE_THUMB)
#define  ARM_SYS_MODE_ARM      (0x0000001FL + ARM_MODE_ARM)


struct ivt_t {
  FP  func;
  UW  prio;
} static int_vector_table[32];

void make_task_context(TCB *tcb)
{
  UW *pstk;

  pstk = (UW *)tcb->stk & ~7;
  /* PC: clear Thumb bit so restore loads a clean address */
  *--pstk = (UW)tcb->task & ~1;
  *--pstk = (UW)0x14141414L; /* LR */
  *--pstk = (UW)0x12121212L; /* R12 */
  *--pstk = (UW)0x11111111L;
  *--pstk = (UW)0x10101010L;
  *--pstk = (UW)0x09090909L;
  *--pstk = (UW)0x08080808L;
  *--pstk = (UW)0x07070707L;
  *--pstk = (UW)0x06060606L;
  *--pstk = (UW)0x05050505L;
  *--pstk = (UW)0x04040404L;
  *--pstk = (UW)0x03030303L;
  *--pstk = (UW)0x02020202L;
  *--pstk = (UW)0x01010101L;
  *--pstk = (UW)tcb->exinf; /* R0 = task argument */

  /* CPSR: SYS mode, IRQ+FIQ enabled; Thumb vs ARM from entry LSB */
  if ((UW)tcb->task & 0x01) {
      *--pstk = (UW)ARM_SYS_MODE_THUMB;
  } else {
      *--pstk = (UW)ARM_SYS_MODE_ARM;
  }
  tcb->tskctxb.sp = pstk;
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
