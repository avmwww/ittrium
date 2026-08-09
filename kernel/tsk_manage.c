/* ittrium kernel — tsk_manage.c
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
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
  
  if ((tskid > (TMAX_TSKID + TRSV_TSKID)) || (tskid < TMIN_TSKID))
    return E_ID;
  
  tcb = get_tcb(tskid);
  if (TTS_NOEXS != tcb->state)
    return E_OBJ;

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
  
  kqueue_remove(&(tcb->qnode));
  tcb->exinf = pk_ctsk->exinf;
  tcb->tskatr = pk_ctsk->tskatr;
  tcb->task = pk_ctsk->task;
  tcb->itskpri = pri_to_index(pk_ctsk->itskpri);
  tcb->stksz = stksz;
  tcb->actcnt = 0;
  tcb->run_ticks = 0;
  tcb->stk_base = stack;
  tcb->stk = (VP)(((VB *) stack) + stksz );
#ifdef USE_SEPARATE_STACK
  tcb->sstk = (VP)(((VB *) sstack) + stksz );
#endif // USE_SEPARATE_STACK
  tcb->name[0] = '\0';
  if ((pk_ctsk->tskatr & TA_NAME) && pk_ctsk->name) {
    UINT ni;
    for (ni = 0; ni + 1 < TSK_NAME_LEN && pk_ctsk->name[ni]; ni++)
      tcb->name[ni] = pk_ctsk->name[ni];
    tcb->name[ni] = '\0';
  }
  
  task_make_dormant(tcb);
  END_CRITICAL_SECTION;
  
  return E_OK;
}

/*
 * Create Task (ID Number Automatic Assignment) — IDs in
 * (TMAX_TSKID+1 .. TMAX_TSKID+TRSV_TSKID), see TRSV_TSKID in kernel_config.
 */
ER_ID acre_tsk(T_CTSK *pk_ctsk)
{
  ID i;
  ER er;

  if (TRSV_TSKID == 0)
    return E_NOID;

  for (i = TMAX_TSKID + 1; i <= TMAX_TSKID + TRSV_TSKID; i++) {
    if (TTS_NOEXS == get_tcb(i)->state) {
      er = cre_tsk(i, pk_ctsk);
      if (er != E_OK)
        return (ER_ID)er;
      return (ER_ID)i;
    }
  }
  return E_NOID;
}

/*
 * Delete task. Target must not be the invoking task. Non-dormant tasks are
 * forced out of ready/wait queues first (needed by elf_kill).
 */
ER del_tsk(ID tskid)
{
  TCB *tcb;
  ER ercd;

  if ((tskid > (TMAX_TSKID + TRSV_TSKID)) || (tskid < TMIN_TSKID))
    return E_ID;

  BEGIN_CRITICAL_SECTION;
  do {
    tcb = get_tcb(tskid);
    if (TTS_NOEXS == tcb->state) {
      ercd = E_NOEXS;
      break;
    }
    if (tcb == runtsk) {
      ercd = E_OBJ;
      break;
    }

    if (tcb->state & TTS_WAI)
      wait_cancel(tcb);

    if (TTS_RDY == tcb->state || TTS_RUN == tcb->state)
      task_make_unready(tcb);

    tcb->state = TTS_NOEXS;
    tcb->actcnt = 0;
    tcb->wait_obj = (OBJCB *)0;
    tcb->name[0] = '\0';
    tcb->stk_base = (VP)0;
    tcb->stksz = 0;
    kqueue_insert(&(tcb->qnode), &free_tcb);
    ercd = E_OK;
  } while (0);
  END_CRITICAL_SECTION;

  if (E_OK == ercd)
    dispatch();
  return ercd;
}

static ER _act_tsk(ID tskid)
{
  TCB    *tcb;
  STAT  state;
  ER ercd;
  
  if ((tskid > (TMAX_TSKID + TRSV_TSKID)) ||
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
      task_make_ready(tcb);
      ercd = E_OK;
    }
  } while (0);
  END_CRITICAL_SECTION;

  return ercd;
}

ER act_tsk(ID tskid)
{
  ER ercd;

  ercd = _act_tsk(tskid);
  if (E_OK == ercd)
    dispatch();
  return ercd;
}

ER iact_tsk(ID tskid)
{
  return _act_tsk(tskid);
}

/**
 *
 */
ER chg_pri(ID tskid, PRI tskpri)
{
  TCB    *tcb;
  STAT  state;

  if ((tskid > (TMAX_TSKID + TRSV_TSKID)) ||
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
    task_change_pri(tcb, tcb->itskpri);
  else
    task_change_pri(tcb, pri_to_index(tskpri));

  END_CRITICAL_SECTION;
  dispatch();
  return E_OK;

// E_ILUSE Illegal service call use (priority ceiling violation)
}

ER rot_rdq(PRI tskpri)
{
  if (TPRI_SELF == tskpri) {
    BEGIN_CRITICAL_SECTION;
    rdyq_rotate_current();
    END_CRITICAL_SECTION;
  } else {
    if (tskpri < TMIN_TPRI || tskpri > TMAX_TPRI)
      return E_PAR;
    BEGIN_CRITICAL_SECTION;
    rdyq_rotate_at(pri_to_index(tskpri));
    END_CRITICAL_SECTION;
  }
  dispatch();
  return E_OK;
}

void ext_tsk(void)
{
  TCB *tcb;

  BEGIN_CRITICAL_SECTION;
  tcb = runtsk;
  task_make_unready(tcb);
  task_make_dormant(tcb);
  if (tcb->actcnt) {
    tcb->actcnt -= 1;
    make_task_context(tcb);
    task_make_ready(tcb);
  }
  runtsk = (TCB *)0;
  END_CRITICAL_SECTION;
  dispatch();
}
//******************************************************************************
