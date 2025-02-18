#include "ittrium.h"
#include "../kernel/task.h"
#include "../cpu_generic.c"


/**
 *
 */
struct ivt_t {
  FP  func;
  UW  prio;
} static int_vector_table[32];


/**
 *
 */
void make_task_context(TCB *tcb)
{
  unsigned short *pstk;

  pstk = (unsigned short *)((unsigned char *)tcb->stk - 4);//- 3); ADi FIX
  // Flag register high part, Program counter high part    ISP  USP
  // IPL = 0, ISP used, Interrupt enabled                  0x40 0xC0
  *--pstk = (((unsigned long)tcb->task >> 8L)&0x0F00)|0x0040;
  // PC medium & low byte
  *--pstk = (unsigned long)tcb->task & 0x0000FFFFL;
  // FB
  *--pstk = 0xFBFB;
  // SB
  *--pstk = 0x5B5B;
  // A1
  *--pstk = 0xA1A1;
  // A0
  *--pstk = (unsigned short)(unsigned int)(tcb->exinf);
  // R3
  *--pstk = 0x3333;
  // R2
  *--pstk = 0x2222;
  // R1
  *--pstk = 0x1111;
  // R0
  *--pstk = 0x0000;

  tcb->tskctxb.sp = pstk;
}

/**
 *
 */
//unsigned short _interrupt_vector;
void interrupt_handler(INHNO vector)
{
  if (int_vector_table[vector].func) {
    cpu_unlock();
    int_vector_table[vector].func();
  }
}

/**
 *
 */
void _install_handler(FP handler, INHNO vec_no, UB prio)
{
  if (vec_no < 32) {
    BEGIN_CRITICAL_SECTION;
    int_vector_table[vec_no].func = handler;
    int_vector_table[vec_no].prio = prio;
    END_CRITICAL_SECTION;
  }
}

/**
 *
 */
void _int_init(void)
{
  int i;

  int_nesting = 0;
  for (i = 0;  i < 32; i++) {
    int_vector_table[i].func = (FP)0;
    int_vector_table[i].prio = 0;
  }
}

/**
 *
 */
void start_hw_timer(void)
{
  timer_nesting = 0;
  INIT_SYSTEM_TICKS();

  install_handler(timer_handler, TICKER_VEC_NO, TICKER_PRIO);
  ENABLE_TICKER_INT();

  INT_VECTOR0 = IRQ_TABLE_BASE & MASK_INDEX;

  // Configure Timer0
  T0LOAD =  TIMER_1MS;	// Timer value
  T0VALUE = TIMER_1MS;
  T0CLR = 0;
  T0CTRL=0xC8;
}

/**
 *
 */
void terminate_hw_timer(void)
{
  DISABLE_TICKER_INT();
  int_vector_table[TICKER_VEC_NO].func = (FP)0;
  int_vector_table[TICKER_VEC_NO].prio = 0;
}

/**
 *
 */
void low_level_init(void)
{
  _int_init();
}
