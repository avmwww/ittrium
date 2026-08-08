/* ittrium kernel — task.c
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "ittrium.h"
#include "task.h"
#include "kqueue.h"
#include "wait.h"
#include "rdyq.h"

TCB  *runtsk;
TCB  *schedtsk;

TCB	tcb_table[TNUM_TSKID];
KQUEUE	free_tcb;

volatile UW telemetry_wall_ticks;
volatile UW telemetry_idle_ticks;


RDYQ	rdyq;

#ifdef BLK_MEM_SIZE
static VB blk_mem[BLK_MEM_SIZE];
VB *pfree_blk_mem;

VP get_blk_mem(INT size)
{
  VP p = (VP)0;

  if (0 == pfree_blk_mem)
    return p;

  BEGIN_CRITICAL_SECTION;

  *((INT *)pfree_blk_mem) = size;
  p = pfree_blk_mem+2;
  pfree_blk_mem += size+2;


  END_CRITICAL_SECTION;
  return p;
}

void free_blk_mem(VP p)
{
  INT size;

  BEGIN_CRITICAL_SECTION;
  size = *((INT *)((VB*)p - 2));
  pfree_blk_mem -= size+2;
  END_CRITICAL_SECTION;
}
#endif // BLK_MEM_SIZE

/*******************************************************************************
 Initialize task's table
 *******************************************************************************/
void task_initialize(void)
{
  ID	i;
  TCB	*tcb;
  ID	tskid;

  runtsk = schedtsk = (TCB *) 0;
  rdyq_init(&rdyq);
  kqueue_init(&free_tcb);

  for(tcb = tcb_table, i = 0; i < TNUM_TSKID; tcb++, i++) {
    tskid = (ID)(i + TMIN_TSKID);
    tcb->tskid = tskid;
    tcb->state = TTS_NOEXS;
    tcb->stk_base = (VP)0;
    tcb->stksz = 0;
    tcb->run_ticks = 0;
    tcb->name[0] = '\0';
    kqueue_insert(&(tcb->qnode), &free_tcb);
  }
#ifdef BLK_MEM_SIZE
  pfree_blk_mem = blk_mem;
#endif // BLK_MEM_SIZE
}


void task_make_dormant(TCB *tcb)
{
   tcb->state = TTS_DMT;
   tcb->tskpri = tcb->itskpri;
   tcb->wupcnt = 0;
   tcb->suscnt = 0;
#ifdef USE_MAILBOX
   tcb->tmq_head = (T_MSG *) 0;
#endif /* USE_MAILBOX */
   if (tcb->stk_base && tcb->stksz) {
     VB *p = (VB *)tcb->stk_base;
     SIZE n = tcb->stksz;
     while (n--)
       *p++ = (VB)0xa5;
   }
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void reschedule(void)
{
  TCB   *toptsk;

  if (schedtsk != (toptsk = rdyq_top(&rdyq)))
    schedtsk = toptsk;
}

/**
 * Move task to READY state and insert to ready queue
 */
void task_make_ready(TCB *tcb)
{
  tcb->state = TTS_RDY;
  if (rdyq_insert(&rdyq, tcb))
    schedtsk = tcb;
}

/**
 * Move task from READY state
 */
void task_make_unready(TCB *tcb)
{
  rdyq_remove(&rdyq, tcb);
  if (schedtsk == tcb)
    schedtsk = rdyq_top(&rdyq);
}

/**
 * Change task priority.
 */
void task_change_pri(TCB *tcb, PRI priority)
{
  rdyq_remove(&rdyq, tcb);
  tcb->tskpri = priority;
  schedtsk = rdyq_top(&rdyq);
  if (rdyq_insert(&rdyq, tcb))
    schedtsk = tcb;
}

/**
 *
 */
void rdyq_rotate_at(PRI priority)
{
   rdyq_rotate(&rdyq, priority);
   reschedule();
}

/**
 *
 */
void rdyq_rotate_current(void)
{
   if (schedtsk)
   {
      rdyq_rotate(&rdyq,
      rdyq_top_pri(&rdyq));
      reschedule();
   }
}
