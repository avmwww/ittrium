/******************************************************************************
**	Filename:		wait.c
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
#include "wait.h"
#include "timer.h"

/**
 *
 */
void wait_release_ok(TCB *tcb)
{
  tcb->ercd = E_OK;
  wait_release(tcb);
}

/**
 *
 */
void wait_release_tmout(TCB *tcb)
{
  queue_delete(&(tcb->tskque));
  move_from_waiting_state(tcb);
}

/**
 *
 */
void make_wait(TMO tmout)
{
  switch (runtsk->state) {
    case TTS_RUN:
    case TTS_RDY:
      move_from_ready_state(runtsk);
      runtsk->state = TTS_WAI;
      break;
    case TTS_SUS:
      runtsk->state = TTS_WAS;
      break;
  }
  timer_insert(&(runtsk->wtmeb), tmout,
               (FP_VP) wait_release_tmout, runtsk);
}

/**
 *
 */
void wait_delete(QUEUE *wait_queue)
{
  TCB *tcb;

  while (!queue_is_empty(wait_queue)) {
    tcb = (TCB *)(wait_queue->next);
    *(tcb->wercd) = E_DLT;
    wait_release(tcb);
  }
}

/**
 *
 */
ID wait_tskid(QUEUE *wait_queue)
{
  if (queue_is_empty(wait_queue))
    return((ID) FALSE);
  else
    return(((TCB *)(wait_queue->next))->tskid);
}

/**
 *
 */
void gcb_make_wait(GCB *gcb, TMO tmout)
{
  runtsk->wgcb = gcb;
  make_wait(tmout);
  if (gcb->objatr & TA_TPRI) {
    // Object in task priority order
    queue_insert_tpri(runtsk, &(gcb->wait_queue));
  }
  else {
    // Object in FIFO order
    queue_insert(&(runtsk->tskque), &(gcb->wait_queue));
  }
}

/**
 *
 */
void obj_chg_pri(TCB *tcb, INT oldpri)
{
  gcb_change_priority(tcb->wgcb, tcb);
}
