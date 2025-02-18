/******************************************************************************
**	Filename:		tsk_manage.c
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


#ifdef BLK_MEM_SIZE
extern VP get_blk_mem(INT size);
extern void free_blk_mem(VP blk);
#endif //  BLK_MEM_SIZE

/**
 *
 */
ER cre_tsk(ID tskid, T_CTSK *pk_ctsk)
{
  VP stack;
#ifdef USE_SEPARATE_STACK
  VP sstack;
#endif // USE_SEPARATE_STACK
  TCB *tcb;
  SIZE stksz;
  
  if ((tskid > TMAX_TSKID) || (tskid < TMIN_TSKID))
    return E_ID;
  
  tcb = get_tcb(tskid);

  stksz = pk_ctsk->stksz;
  stack = (VP)0;

  if ((VP)0 == pk_ctsk->stk) {
#ifdef BLK_MEM_SIZE
    if((VP)0 == (stack = get_blk_mem(stksz)))
#endif // BLK_MEM_SIZE
      return E_NOMEM;
  }

#ifdef USE_SEPARATE_STACK
  sstack = (VP)0;
  if ((VP)0 == pk_ctsk->sstk) {
#ifdef BLK_MEM_SIZE
    if((VP)0 == (sstack = get_blk_mem(stksz))) {
      if ((VP)0 != stack)
        free_blk_mem(stack);
      return E_NOMEM;
    }
#else
      return E_NOMEM;
#endif // BLK_MEM_SIZE
  }
  if ((VP)0 == sstack)
    sstack = pk_ctsk->sstk;
#endif // USE_SEPARATE_STACK

  if ((VP)0 == stack)
    stack = pk_ctsk->stk;

  BEGIN_CRITICAL_SECTION;
  
  queue_delete(&(tcb->tskque));
  tcb->exinf = pk_ctsk->exinf;
  tcb->tskatr = pk_ctsk->tskatr;
  tcb->task = pk_ctsk->task;
  tcb->itskpri = int_priority(pk_ctsk->itskpri);
  tcb->stksz = stksz;
  tcb->actcnt = 0;
  
  tcb->stk = (VP)(((VB *) stack) + stksz );
#ifdef USE_SEPARATE_STACK
  tcb->sstk = (VP)(((VB *) sstack) + stksz );
#endif // USE_SEPARATE_STACK
  
  make_dormant(tcb);
  END_CRITICAL_SECTION;
  
  return E_OK;
}

/**
 *
 */
ER act_tsk(ID tskid)
{
  TCB    *tcb;
  STAT  state;
  ER ercd;
  
  if ((tskid > TMAX_TSKID) ||
      ((TSK_SELF == tskid)&&((TCB*)0 == runtsk))
     )
     return E_ID;
  
  BEGIN_CRITICAL_SECTION;
  do {
    tcb = get_tcb_self(tskid);
    if ((state = tcb->state) == TTS_NOEXS) {
      ercd = E_NOEXS;
      break;
    }
  
    if (state != TTS_DMT) {
      if (tcb->actcnt >= TMAX_ACTCNT) {
        ercd = E_QOVR;
      } else {
        tcb->actcnt += 1;
        ercd = E_OK;
      }
    } else {
      make_task_context(tcb);
      move_to_ready_state(tcb);
      ercd = E_OK;
    }
  } while (0);
  END_CRITICAL_SECTION;

  if (E_OK == ercd) dispatch();
  return ercd;
}

/**
 *
 */
ER chg_pri(ID tskid, PRI tskpri)
{
  TCB    *tcb;
  STAT  state;

  if ((tskid > TMAX_TSKID) ||
      ((TSK_SELF == tskid)&&((TCB*)0 == runtsk)))
    return E_ID;

  if ((tskpri<TMIN_TPRI || tskpri>TMAX_TPRI) && tskpri != TPRI_INI)
    return E_PAR;

  tcb = get_tcb_self(tskid);
  if ((state = tcb->state) == TTS_NOEXS)
    return E_NOEXS;
  if (TTS_DMT == state)
    return E_OBJ;

  BEGIN_CRITICAL_SECTION;
  if (TPRI_INI == tskpri)
    change_task_priority(tcb, tcb->itskpri);
  else
    change_task_priority(tcb, int_priority(tskpri));

  END_CRITICAL_SECTION;
  dispatch();
  return E_OK;

// E_ILUSE Illegal service call use (priority ceiling violation)
}

/*
 *
 */
#ifdef ittrium_ext_tsk
void ext_tsk(void)
{
  BEGIN_CRITICAL_SECTION;
  move_from_ready_state(runtsk);
  make_dormant(runtsk);
  if (runtsk->actcnt) {
    runtsk->actcnt -= 1;
    move_to_ready_state(runtsk);
    runtsk = (TCB *)0;
  }
  END_CRITICAL_SECTION;
  dispatch();
}

#endif // ittrium_ext_tsk
//******************************************************************************
