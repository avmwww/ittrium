/******************************************************************************
**	Filename:		wait.h
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
#ifndef _WAIT_H_
#define _WAIT_H_

#include "task.h"
#include "queue.h"
#include "timer.h"

//#define	offsetof(structure, field) ((INT) &(((structure *) 0)->field))

extern void wait_release_ok(TCB *tcb);
extern void wait_release_tmout(TCB *tcb);

/**
 * Move task from WAITING/WAITING-SUSPENDED state.
 * WAITING          : READY
 * WAITING-SUSPENDED: SUSPENDED
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void move_from_waiting_state(TCB *tcb)
{
  if (tcb->state == TTS_WAI)
    move_to_ready_state(tcb);
  else
    tcb->state = TTS_SUS;
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void wait_release(TCB *tcb)
{
  timer_delete(&(tcb->wtmeb));
  queue_delete(&(tcb->tskque));
  move_from_waiting_state(tcb);
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void wait_cancel(TCB *tcb)
{
  timer_delete(&(tcb->wtmeb));
  queue_delete(&(tcb->tskque));
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
QUEUE *queue_search_max_tpri(QUEUE *queue, PRI prio)
{
  QUEUE *entry;

  for (entry = queue->next; entry != queue; entry = entry->next) {
    if (((TCB *)entry)->tskpri > prio) {
      break;
    }
  }
  return(entry);
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void queue_insert_tpri(TCB *tcb, QUEUE *queue)
{
   QUEUE   *q;

   q = queue_search_max_tpri(queue, tcb->tskpri);
   queue_insert(&(tcb->tskque), q);
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void gcb_change_priority(GCB *gcb, TCB *tcb)
{
   queue_delete(&(tcb->tskque));
   queue_insert_tpri(tcb, &(gcb->wait_queue));
}

extern void make_wait(TMO tmout);
extern void wait_delete(QUEUE *wait_queue);
extern ID   wait_tskid(QUEUE *wait_queue);
extern void gcb_make_wait(GCB *gcb, TMO tmout);
extern void obj_chg_pri(TCB *tcb, INT oldpri);

#endif /* _WAIT_H_ */
