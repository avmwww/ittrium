#include "ittrium.h"

void install_handler(FP handler, INHNO vec_no)
{
  BEGIN_CRITICAL_SECTION;
  _install_handler(handler, vec_no);
  END_CRITICAL_SECTION;
}

#if 0
#include "../kernel/task.h"
void make_task_context(TCB *tcb)
{
   VH *stk = tcb->stk;
   VH *sstk = tcb->sstk;
   ID tskid = tcb->tskid;
   SIZE i;
   VH *temp;
   
   // Clear stack & system stack
   stk -= tcb->stksz -1;
   sstk -= tcb->stksz - 1;
   for (i=0; i < tcb->stksz; i++)
   {
      *stk++ = 0;
      *sstk++ = 0;
   }
   stk -= 4;
   sstk -= 4;
   // ST0_55
   sstk[2] = 0x3800;
   // ST1_55
   stk[1] = 0x6100;
   // ST2_55
   stk[2] = 0xF000;
   // ST3_55
   // 0x5C04
   // RETA
   stk[0] = (VH)tcb->task;
   sstk[0] = (VH)(((VW)tcb->task)>>16);
/*
   // SYSTEM stacks
   stk = &_tsk_stack[tskid<<6];
   sstk = &_tsk_system_stack[tskid<<6];
   for (i=0; i<64; i++)
   {
      *stk++ = 0;
      *sstk++ = 0;
   }
   // Prepare T0
   if (0 != tcb->exinf)
   {
       stk = &_tsk_stack[tskid<<6] + (64-13);
       *stk = *(tcb->exinf);
   }
   
   // Prepare SP SSP RETA
   stk = &_tsk_stack[tskid<<6] + (64-46);
   sstk = &_tsk_system_stack[tskid<<6] + (64-46);
   // SP
   temp = tcb->stk;
   temp -=3;
   *--stk = (VH)temp;
   *--sstk = (VH)(((VW)temp)>>16);
   // SSP
   temp = tcb->sstk;
   temp -=3;
   *--stk = (VH)temp;
   *--sstk = (VH)(((VW)temp)>>16);
   // RETA
   *--stk = (VH)tcb->task;
   *--sstk = (VH)(((VW)tcb->task)>>16);
*/
}
#endif
