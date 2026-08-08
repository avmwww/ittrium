/* ittrium kernel — tsk_dep_syn.c
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "ittrium.h"
#include "kqueue.h"
#include "timer.h"
#include "wait.h"
#include "task.h"

ER dly_tsk(RELTIM dlytim)
{
  BEGIN_CRITICAL_SECTION;
  runtsk->ercd = E_OK;
  runtsk->wait_obj = (OBJCB *) 0;
  make_wait(dlytim);
  kqueue_init(&(runtsk->qnode));
  END_CRITICAL_SECTION;

  dispatch();
  return runtsk->ercd;
}

ER sus_tsk(ID tskid)
{
  TCB *tcb;
  ER ercd;

  if ((tskid > (TMAX_TSKID + TRSV_TSKID)) ||
      ((TSK_SELF == tskid)&&((TCB*)0 == runtsk)))
    return E_ID;

  tcb = get_tcb_self(tskid);

  BEGIN_CRITICAL_SECTION;

  if (TTS_NOEXS == tcb->state)
    ercd = E_NOEXS;
  else if (TTS_DMT == tcb->state)
    ercd = E_OBJ;
  else if (TMAX_ACTCNT == tcb->suscnt)
    ercd = E_QOVR;
  else {
    ++(tcb->suscnt);
    if (TTS_RUN == tcb->state || TTS_RDY == tcb->state) {
      tcb->state = TTS_SUS;
      task_make_unready(tcb);
    } else {
      // Waiting state
      tcb->state = TTS_WAS;
    }
    ercd = E_OK;
  }
  END_CRITICAL_SECTION;
  dispatch();
  return ercd;
}

ER rsm_tsk(ID tskid)
{
  TCB *tcb;
  ER ercd;

  if ((tskid > (TMAX_TSKID + TRSV_TSKID)) ||
      ((TSK_SELF == tskid)&&((TCB*)0 == runtsk)))
    return E_ID;

  tcb = get_tcb_self(tskid);

  BEGIN_CRITICAL_SECTION;
  if (TTS_NOEXS == tcb->state)
    ercd = E_NOEXS;
  else if (TTS_SUS != tcb->state && TTS_WAS != tcb->state)
    ercd = E_OBJ;
  else {
    if (--(tcb->suscnt) == 0) {
      if (TTS_WAS == tcb->state)
        tcb->state = TTS_WAI;
      else
        task_make_ready(tcb);
    }
    ercd = E_OK;
  }
  END_CRITICAL_SECTION;
  dispatch();
  return ercd;
}
