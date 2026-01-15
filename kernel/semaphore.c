/******************************************************************************
**	Filename:		semaphore.c
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
#include "queue.h"

#ifdef USE_SEMAPHORE

typedef struct semaphore_control_block {
   GCB   gcb;
   INT   semcnt;
   INT   maxsem;
} SEMCB;

static SEMCB semcb_table[TNUM_SEMID];

#define get_semcb_by_id(id)   (&(semcb_table[(id) - TMIN_SEMID]))

QUEUE free_semcb;

void semaphore_initialize()
{
   ID       i;
   SEMCB   *semcb;
   ID       semid;

   queue_initialize(&free_semcb);

   for(semcb = semcb_table, i = 0; i < TNUM_SEMID; semcb++, i++) {
      semid = i;
      semcb->gcb.objid = semid;
      semcb->gcb.state = TTS_NOEXS;
      queue_insert(&(semcb->gcb.wait_queue), &free_semcb);
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
  if (TTS_NOEXS != semcb->gcb.state)
     return E_OBJ;
  
  BEGIN_CRITICAL_SECTION;
  queue_delete(&(semcb->gcb.wait_queue));
  queue_initialize(&(semcb->gcb.wait_queue));
  semcb->gcb.objatr = pk_csem->sematr;
  semcb->gcb.state = TTS_RDY;
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

  if (TRSV_SEMID == 0)
    return E_NOID;

  for (i = TMAX_SEMID + 1; i < TMAX_SEMID + 1; i++) {
    semcb = get_semcb_by_id(semid);
    if (TTS_NOEXS == semcb->gcb.state)
      return _cre_sem(i, pk_csem);
  }
  return E_NOID;
}

static ER __wai_sem(ID semid, TMO tmout)
{
  SEMCB   *semcb;

  if (semid < TMIN_SEMID || semid > TMAX_SEMID)
    return E_ID;
  
  semcb = get_semcb_by_id(semid);
  if (TTS_NOEXS == semcb->gcb.state)
    return E_OBJ;
  
  BEGIN_CRITICAL_SECTION;
  if (0 < semcb->semcnt) {
    semcb->semcnt -= 1;
    runtsk->ercd = E_OK;
  }
  else {
    runtsk->ercd = E_TMOUT;
    if (TMO_POL != tmout)
      gcb_make_wait(&(semcb->gcb), tmout);
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
  if (TTS_NOEXS == semcb->gcb.state)
    return E_OBJ;
  
  BEGIN_CRITICAL_SECTION;
  if (!queue_is_empty(&(semcb->gcb.wait_queue)))
    wait_release_ok((TCB *)(semcb->gcb.wait_queue.next));
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

