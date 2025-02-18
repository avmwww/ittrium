#include "ittrium.h"
#include "../kernel/task.h"


#ifndef INIT_SYSTEM_TICKS
# define INIT_SYSTEM_TICKS()  {\
  _system_ticks.st_low = 0;    \
  _system_ticks.st_high = 0;   \
                              }
#endif // INIT_SYSTEM_TICKS

__no_init SYSTIM _system_ticks;

void make_task_context(TCB *tcb)
{
  unsigned short *pstk;

  pstk = (unsigned short *)((unsigned char *)tcb->stk - 3);
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
  *--pstk = (unsigned short)(tcb->exinf);
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


void timer_handler(void);

static FP _int_vector_table[64];

static char timer_nesting;
void start_hw_timer()
{
  _int_vector_table[TICKER_VEC_NO] = (FP)timer_handler;
  timer_nesting = 0;
  INIT_SYSTEM_TICKS();
  ENABLE_TICKER_INT();
}

void terminate_hw_timer()
{
  DISABLE_TICKER_INT();
  _int_vector_table[TICKER_VEC_NO] = (FP)0;
}

int timer_hw_enter()
{
  int ret;

  BEGIN_CRITICAL_SECTION;
  _system_ticks.st_low++;
  if (TIC_DENO == _system_ticks.st_low) {
    _system_ticks.st_low = 0;
    _system_ticks.st_high++;
  }
  if (timer_nesting) ret = 1;
  else {
    timer_nesting++;
    ret = 0;
  }
  END_CRITICAL_SECTION;
  return ret;
}

void timer_hw_exit()
{
  timer_nesting--;
}


unsigned short _interrupt_vector;
void interrupt_handler(INHNO vector)
{
  cpu_unlock();
  if (_int_vector_table[vector]) _int_vector_table[vector]();
}

void _install_handler(FP handler, INHNO vec_no)
{
  if (vec_no < 64) {
    BEGIN_CRITICAL_SECTION;
    _int_vector_table[vec_no] = handler;
    END_CRITICAL_SECTION;
  }
}

char int_nesting;
void _int_init(void)
{
  int i;

  int_nesting = 0;
  for (i = 0;  i < 64; i++) _int_vector_table[i] = (FP)0;

}

