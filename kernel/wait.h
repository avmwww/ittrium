/* ittrium kernel — wait.h
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef _WAIT_H_
#define _WAIT_H_

#include "task.h"
#include "kqueue.h"
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
void task_leave_wait(TCB *tcb)
{
  tcb->tskwait = 0;
  if (tcb->state == TTS_WAI)
    task_make_ready(tcb);
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
  kqueue_remove(&(tcb->qnode));
  task_leave_wait(tcb);
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void wait_cancel(TCB *tcb)
{
  timer_delete(&(tcb->wtmeb));
  kqueue_remove(&(tcb->qnode));
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
KQUEUE *kqueue_find_pri(KQUEUE *queue, PRI prio)
{
  KQUEUE *entry;

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
void kqueue_insert_pri(TCB *tcb, KQUEUE *queue)
{
   KQUEUE   *q;

   q = kqueue_find_pri(queue, tcb->tskpri);
   kqueue_insert(&(tcb->qnode), q);
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void obj_change_pri(OBJCB *gcb, TCB *tcb)
{
   kqueue_remove(&(tcb->qnode));
   kqueue_insert_pri(tcb, &(gcb->waitq));
}

extern void make_wait(TMO tmout);
extern void wait_delete(KQUEUE *waitq);
extern ID   wait_tskid(KQUEUE *waitq);
extern void obj_make_wait(OBJCB *gcb, TMO tmout);
extern void obj_chg_pri(TCB *tcb, INT oldpri);

#endif /* _WAIT_H_ */
