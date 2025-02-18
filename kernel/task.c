/******************************************************************************
**	Filename:		task.c
**	Purpose:		
**	Author:			Andrey Mitrofanov
**	Environment:		ittrium (mITRON) Real Time Kernel
**	History:
**
**	Version:		1.0
**	Notes:			
**
**	(c) 2004 Copyright Andrey Mitrofanov.
**	Copying or other reproduction of
**	this program except for archival purposes is prohibited
**	without the prior written consent of author.
*******************************************************************************/
#include "ittrium.h"
#include "task.h"
#include "queue.h"
#include "wait.h"
#include "ready_queue.h"

TCB  *runtsk;
TCB  *schedtsk;

TCB	tcb_table[TNUM_TSKID];
QUEUE	free_tcb;


RDYQUE	ready_queue;

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
  ready_queue_initialize(&ready_queue);
  queue_initialize(&free_tcb);

  for(tcb = tcb_table, i = 0; i < TNUM_TSKID; tcb++, i++) {
    tskid = i;
    tcb->tskid = tskid;
    tcb->state = TTS_NOEXS;
    queue_insert(&(tcb->tskque), &free_tcb);
  }
#ifdef BLK_MEM_SIZE
  pfree_blk_mem = blk_mem;
#endif // BLK_MEM_SIZE
}


void make_dormant(TCB *tcb)
{
   tcb->state = TTS_DMT;
   tcb->tskpri = tcb->itskpri;
   tcb->wupcnt = 0;
   tcb->suscnt = 0;
#ifdef USE_MAILBOX
   tcb->tmq_head = (T_MSG *) 0;
#endif /* USE_MAILBOX */
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void reschedule(void)
{
  TCB   *toptsk;

  if (schedtsk != (toptsk = ready_queue_top(&ready_queue)))
    schedtsk = toptsk;
}

/**
 * Move task to READY state and insert to ready queue
 */
void move_to_ready_state(TCB *tcb)
{
  tcb->state = TTS_RDY;
  if (ready_queue_insert(&ready_queue, tcb))
    schedtsk = tcb;
}

/**
 * Move task from READY state
 */
void move_from_ready_state(TCB *tcb)
{
  ready_queue_delete(&ready_queue, tcb);
  if (schedtsk == tcb)
    schedtsk = ready_queue_top(&ready_queue);
}

/**
 * Change task priority.
 */
void change_task_priority(TCB *tcb, PRI priority)
{
  ready_queue_delete(&ready_queue, tcb);
  tcb->tskpri = priority;
  schedtsk = ready_queue_top(&ready_queue);
  if (ready_queue_insert(&ready_queue, tcb))
    schedtsk = tcb;
}

/**
 *
 */
void rotate_ready_queue(PRI priority)
{
   ready_queue_rotate(&ready_queue, priority);
   reschedule();
}

/**
 *
 */
void rotate_ready_queue_run(void)
{
   if (schedtsk)
   {
      ready_queue_rotate(&ready_queue,
      ready_queue_top_priority(&ready_queue));
      reschedule();
   }
}
