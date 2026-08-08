/* ittrium kernel — wait.c
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
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
  kqueue_remove(&(tcb->qnode));
  task_leave_wait(tcb);
}

/**
 *
 */
void make_wait(TMO tmout)
{
  switch (runtsk->state) {
    case TTS_RUN:
    case TTS_RDY:
      task_make_unready(runtsk);
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
void wait_delete(KQUEUE *waitq)
{
  TCB *tcb;

  while (!kqueue_empty(waitq)) {
    tcb = (TCB *)(waitq->next);
    *(tcb->wercd) = E_DLT;
    wait_release(tcb);
  }
}

/**
 *
 */
ID wait_tskid(KQUEUE *waitq)
{
  if (kqueue_empty(waitq))
    return((ID) FALSE);
  else
    return(((TCB *)(waitq->next))->tskid);
}

/**
 *
 */
void obj_make_wait(OBJCB *gcb, TMO tmout)
{
  runtsk->wait_obj = gcb;
  make_wait(tmout);
  if (gcb->objatr & TA_TPRI) {
    // Object in task priority order
    kqueue_insert_pri(runtsk, &(gcb->waitq));
  }
  else {
    // Object in FIFO order
    kqueue_insert(&(runtsk->qnode), &(gcb->waitq));
  }
}

/**
 *
 */
void obj_chg_pri(TCB *tcb, INT oldpri)
{
  obj_change_pri(tcb->wait_obj, tcb);
}
