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
  runtsk->tskwait = TTW_DLY;
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

static ER __slp_tsk(TMO tmout)
{
  BEGIN_CRITICAL_SECTION;
  if (runtsk->wupcnt > 0) {
    runtsk->wupcnt -= 1;
    runtsk->ercd = E_OK;
  } else {
    runtsk->ercd = E_TMOUT;
    if (TMO_POL != tmout) {
      runtsk->wait_obj = (OBJCB *)0;
      runtsk->tskwait = TTW_SLP;
      make_wait(tmout);
      kqueue_init(&(runtsk->qnode));
    }
  }
  END_CRITICAL_SECTION;

  dispatch();
  return runtsk->ercd;
}

ER slp_tsk(void)
{
  return __slp_tsk(TMO_FEVR);
}

ER tslp_tsk(TMO tmout)
{
  return __slp_tsk(tmout);
}

static ER _wup_tsk(ID tskid)
{
  TCB *tcb;
  ER ercd;

  if ((tskid > (TMAX_TSKID + TRSV_TSKID)) ||
      ((TSK_SELF == tskid)&&((TCB*)0 == runtsk)))
    return E_ID;

  BEGIN_CRITICAL_SECTION;
  tcb = get_tcb_self(tskid);
  if (TTS_NOEXS == tcb->state) {
    ercd = E_NOEXS;
  } else if (TTS_DMT == tcb->state) {
    ercd = E_OBJ;
  } else if ((tcb->state & TTS_WAI) && (tcb->tskwait == TTW_SLP)) {
    wait_release_ok(tcb);
    ercd = E_OK;
  } else if (tcb->wupcnt >= TMAX_ACTCNT) {
    ercd = E_QOVR;
  } else {
    tcb->wupcnt += 1;
    ercd = E_OK;
  }
  END_CRITICAL_SECTION;

  return ercd;
}

ER wup_tsk(ID tskid)
{
  ER ercd;

  ercd = _wup_tsk(tskid);
  if (E_OK == ercd)
    dispatch();
  return ercd;
}

ER iwup_tsk(ID tskid)
{
  return _wup_tsk(tskid);
}

ER can_wup(ID tskid, INT *p_wupcnt)
{
  TCB *tcb;
  ER ercd;

  if ((tskid > (TMAX_TSKID + TRSV_TSKID)) ||
      ((TSK_SELF == tskid)&&((TCB*)0 == runtsk)))
    return E_ID;

  BEGIN_CRITICAL_SECTION;
  tcb = get_tcb_self(tskid);
  if (TTS_NOEXS == tcb->state) {
    ercd = E_NOEXS;
  } else if (TTS_DMT == tcb->state) {
    ercd = E_OBJ;
  } else {
    if (p_wupcnt)
      *p_wupcnt = tcb->wupcnt;
    tcb->wupcnt = 0;
    ercd = E_OK;
  }
  END_CRITICAL_SECTION;

  return ercd;
}
