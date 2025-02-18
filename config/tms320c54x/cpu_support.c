#include "ittrium.h"

FP int_vector_table[32];

void install_handler(FP handler, INHNO vec_no)
{
  BEGIN_CRITICAL_SECTION;
  int_vector_table[vec_no & 31] = handler;
  END_CRITICAL_SECTION;
}
int int_vec_no;
int int_nesting;

SYSTIM _system_ticks;

const UH int_mask_table[32] = {
0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xfffe,0xfffd,0xfffb,0xfff7,0xffef,0xffdf,0xffbf,0xff7f,
0xfeff,0xfdff,0xfbff,0xf7ff,0xefff,0xdfff,0xbfff,0x7fff
};

int timer_hw_enter(void)
{
  // ??????? Timer nesting not implemented ???????
  BEGIN_CRITICAL_SECTION;
  _system_ticks.st_low++;
  if (TIC_DENO == _system_ticks.st_low) _system_ticks.st_high++;
  END_CRITICAL_SECTION;
  return 0;
}
void timer_hw_exit(void)
{
  // ??????? Timer nesting not implemented ???????
}

void make_task_context(TCB *tcb)
{
   VH *stk = tcb->stk;
   SIZE i;
   
   // Clear stack & system stack
   stk -= 24;
   for (i = 0; i < 24; i++)
   {
      *stk++ = 0;
   }
   stk -= 3;
   // ST1  0x4000
   // BRAF = 0, CPL = 1, XF = 0, HM = 0, INTM = 0, OVM = 0, SXM = 0, C16 = 0,
   // FRCT = 0, CMPT = 0, ASM = 00000
   stk[0] = 0x4000;
   // ST0
   stk[1] = 0x0000;
   // PC
   stk[2] = (VH)tcb->task;

   tcb->tskctxb.sp = stk - 20;

   /*
        PMST  23
        REA   22
        RSA   21
        BRC   20
        BK    19
        TRN   18
        T     17
        ST1   16
        BG    15
        BH    14
        BL    13
        AG    12
        AH    11
        AL    10
        AR7    9
        AR6    8
        AR5    7
        AR4    6
        AR3    5
        AR2    4
        AR1    3
        AR0    2
        ST0    1

        PC     0
  */
}

void void_interrupt_handler(void)
{
}

void low_level_init(void)
{
  int i;

  PMST = 0x00A0;
  for (i = 0; i < 32; i++) {
    int_vector_table[i] = void_interrupt_handler;
  }
  int_vec_no = 0;
  int_nesting = 0;

  _system_ticks.st_low = 0;
  _system_ticks.st_high = 0;
}

