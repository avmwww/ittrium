#include "ittrium.h"
#include "../kernel/task.h"


#ifndef INIT_SYSTEM_TICKS
# define INIT_SYSTEM_TICKS()  {\
  _system_ticks.st_low = 0;    \
  _system_ticks.st_high = 0;   \
                              }
#endif

__no_init SYSTIM _system_ticks;

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


void timer_handler(void);

static FP _int_vector_table[64];

static char timer_nesting;
void start_hw_timer()
{
  if ( install_IRQ(5, 1, timer_handler) == FALSE )
  {
    while(1);
  }
  INT_REQ5 = (1<<28)|(1<<27)|(1<<26)|(1<<16)|0x1;
  INT_VECTOR0 = IRQ_TABLE_BASE & MASK_INDEX;

  T0LOAD =  TIMER_1MS;
  T0VALUE = TIMER_1MS;
  T0CLR = 0;
  T0CTRL=0xC8;

  timer_nesting = 0;
  INIT_SYSTEM_TICKS();
}

void terminate_hw_timer()
{
  INT_REQ5 = (1<<28)|(1<<27)|(1<<26)|(1<<16)|0x0;
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
  T0CLR=0x0;
}

void _install_handler(FP handler, INHNO vec_no)
{
  if (vec_no < 32) {
    BEGIN_CRITICAL_SECTION;
    install_IRQ(vec_no, 1, handler);
    END_CRITICAL_SECTION;
  }
}

char int_nesting;
void _int_init(void)
{
  init_interrupt();
}

void install_handler(FP handler, INHNO vec_no)
{
  _install_handler(handler, vec_no);
}
