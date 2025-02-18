/******************************************************************************
**	Filename:		tsk_dep_syn.c
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
#include "queue.h"
#include "timer.h"
#include "wait.h"
#include "task.h"

ER dly_tsk(RELTIM dlytim)
{
  BEGIN_CRITICAL_SECTION;
  runtsk->ercd = E_OK;
  runtsk->wgcb = (GCB *) 0;
  make_wait(dlytim);
  queue_initialize(&(runtsk->tskque));
  END_CRITICAL_SECTION;

  dispatch();
  return runtsk->ercd;
}

ER sus_tsk(ID tskid)
{
  TCB *tcb;
  ER ercd;

  if ((tskid > TMAX_TSKID) ||
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
      move_from_ready_state(tcb);
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

  if ((tskid > TMAX_TSKID) ||
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
        move_to_ready_state(tcb);
    }
    ercd = E_OK;
  }
  END_CRITICAL_SECTION;
  dispatch();
  return ercd;
}
