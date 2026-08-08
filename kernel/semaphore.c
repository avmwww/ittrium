/* ittrium kernel — semaphore.c
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "ittrium.h"
#include "task.h"
#include "wait.h"
#include "kqueue.h"

#ifdef USE_SEMAPHORE

typedef struct semaphore_control_block {
   OBJCB obj;
   INT   semcnt;
   INT   maxsem;
} SEMCB;

static SEMCB semcb_table[TNUM_SEMID];

#define get_semcb_by_id(id)   (&(semcb_table[(id) - TMIN_SEMID]))

KQUEUE free_semcb;

void semaphore_initialize()
{
   ID       i;
   SEMCB   *semcb;
   ID       semid;

   kqueue_init(&free_semcb);

   for(semcb = semcb_table, i = 0; i < TNUM_SEMID; semcb++, i++) {
      semid = i;
      semcb->obj.objid = semid;
      semcb->obj.state = TTS_NOEXS;
      kqueue_insert(&(semcb->obj.waitq), &free_semcb);
   }
}
//==============================================================================
static ER _cre_sem(ID semid, T_CSEM *pk_csem)
{
  SEMCB *semcb;
  
  if ((pk_csem->sematr &(TA_TFIFO|TA_TPRI))!= pk_csem->sematr )
    return E_RSATR;

  if ((TMAX_MAXSEM < pk_csem->maxsem) || (TMAX_MAXSEM < pk_csem->isemcnt))
    return E_PAR;
  
  semcb = get_semcb_by_id(semid);
  if (TTS_NOEXS != semcb->obj.state)
     return E_OBJ;
  
  BEGIN_CRITICAL_SECTION;
  kqueue_remove(&(semcb->obj.waitq));
  kqueue_init(&(semcb->obj.waitq));
  semcb->obj.objatr = pk_csem->sematr;
  semcb->obj.state = TTS_RDY;
  semcb->semcnt = pk_csem->isemcnt;
  semcb->maxsem = pk_csem->maxsem;
  END_CRITICAL_SECTION;
  return E_OK;
}

ER cre_sem(ID semid, T_CSEM *pk_csem)
{
  if (semid < TMIN_SEMID || semid > (TMAX_SEMID + TRSV_SEMID))
    return E_ID;

  return _cre_sem(semid, pk_csem);
}

/*
 * Create Semaphore (ID Number Automatic Assignment)
 */
ER_ID acre_sem(T_CSEM *pk_csem)
{
  ID i;
  SEMCB *semcb;
  ER err;

  if (TRSV_SEMID == 0)
    return E_NOID;

  for (i = TMAX_SEMID + 1; i < TMAX_SEMID + TRSV_SEMID + 1; i++) {
    semcb = get_semcb_by_id(i);
    if (TTS_NOEXS == semcb->obj.state) {
      err = _cre_sem(i, pk_csem);
      if (err != E_OK)
        return err;
      return i;
    }
  }
  return E_NOID;
}

static ER __wai_sem(ID semid, TMO tmout)
{
  SEMCB   *semcb;

  if (semid < TMIN_SEMID || semid > TMAX_SEMID)
    return E_ID;
  
  semcb = get_semcb_by_id(semid);
  if (TTS_NOEXS == semcb->obj.state)
    return E_OBJ;
  
  BEGIN_CRITICAL_SECTION;
  if (0 < semcb->semcnt) {
    semcb->semcnt -= 1;
    runtsk->ercd = E_OK;
  }
  else {
    runtsk->ercd = E_TMOUT;
    if (TMO_POL != tmout)
      obj_make_wait(&(semcb->obj), tmout);
  }
  END_CRITICAL_SECTION;
  dispatch();
  
  return runtsk->ercd;
}
//==============================================================================
ER wai_sem(ID semid)
{
  return __wai_sem(semid, TMO_FEVR);
}
//==============================================================================
ER twai_sem(ID semid, TMO tmout)
{
  return __wai_sem(semid, tmout);
}
//==============================================================================
ER pol_sem(ID semid)
{
  return __wai_sem(semid, TMO_POL);
}
//==============================================================================
// Release Semaphore Resource
//==============================================================================
static ER _sig_sem(ID semid)
{
  SEMCB   *semcb;
  ER ercd = E_OK;
  
  if (semid < TMIN_SEMID || semid > TMAX_SEMID)
    return E_ID;
  
  semcb = get_semcb_by_id(semid);
  if (TTS_NOEXS == semcb->obj.state)
    return E_OBJ;
  
  BEGIN_CRITICAL_SECTION;
  if (!kqueue_empty(&(semcb->obj.waitq)))
    wait_release_ok((TCB *)(semcb->obj.waitq.next));
  else {
    if (semcb->semcnt >= semcb->maxsem)
      ercd = E_QOVR;
    else
      semcb->semcnt += 1;
  }
  END_CRITICAL_SECTION;
  return ercd;
}

ER sig_sem(ID semid)
{
  ER ercd;
  ercd = _sig_sem(semid);
  dispatch();
  return ercd;
}

ER isig_sem(ID semid)
{
  return _sig_sem(semid);
}

#endif // USE_SEMAPHORE

