/* ittrium kernel — eventflags.c
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "ittrium.h"
#include "task.h"
#include "kqueue.h"
#include "wait.h"

#ifdef USE_EVENTFLAG

#define get_flgcb_by_id(id)   (&(flgcb_table[(id) - TMIN_FLGID]))

typedef struct eventflag_control_block {
   OBJCB obj;
   FLGPTN  flgptn;
} FLGCB;

static FLGCB   flgcb_table[TNUM_FLGID];

/*******************************************************************************
 Startup initialize
 *******************************************************************************/
void eventflag_initialize(void)
{
  ID      id;
  FLGCB   *flgcb;
  ID      flgid;

  for(flgcb = flgcb_table, id = 0; id < TNUM_FLGID; flgcb++, id++) {
    flgid = id;
    flgcb->obj.objid = flgid;
    flgcb->obj.state = TTS_NOEXS;
    kqueue_init(&(flgcb->obj.waitq));
  }
}
//==============================================================================
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
BOOL eventflag_cond(FLGCB *flgcb, FLGPTN waiptn, MODE wfmode)
{
  if (wfmode & TWF_ORW)
    return((flgcb->flgptn & waiptn) != 0);
  else
    return((flgcb->flgptn & waiptn) == waiptn);
}
//==============================================================================
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void clear_flgptn(FLGCB *flgcb)
{
  if (flgcb->obj.objatr & TA_CLR)
     flgcb->flgptn = 0;
}
/*******************************************************************************
 *******************************************************************************/
ER cre_flg(ID flgid, T_CFLG *pk_cflg)
{
  FLGCB *flgcb;
  ER ercd;
   
  if (flgid < TMIN_FLGID || flgid > TMAX_FLGID)
    return E_ID;

  if (pk_cflg->flgatr !=
      (pk_cflg->flgatr & ((TA_TFIFO|TA_TPRI) | (TA_WSGL|TA_WMUL) |TA_CLR)))
    return E_RSATR;
   
  flgcb = get_flgcb_by_id(flgid);
   

  BEGIN_CRITICAL_SECTION;
  if (TTS_NOEXS != flgcb->obj.state)
    ercd = E_OBJ;
  else {
    kqueue_init(&(flgcb->obj.waitq));
    flgcb->obj.objatr = pk_cflg->flgatr;
    flgcb->flgptn = pk_cflg->iflgptn;
    flgcb->obj.state = TTS_RDY;
    ercd = E_OK;
  }
  END_CRITICAL_SECTION;
   
  return ercd;
}
//==============================================================================
ER _set_flg(ID flgid, FLGPTN setptn)
{
  FLGCB *flgcb;
  KQUEUE   *queue;
  TCB *tcb;
  MODE wfmode;
  ER err;
  
  if (flgid < TMIN_FLGID || flgid > TMAX_FLGID)
    return E_ID;
  
  BEGIN_CRITICAL_SECTION;
  flgcb = get_flgcb_by_id(flgid);
  
  if (TTS_NOEXS == flgcb->obj.state) {
    err = E_NOEXS;
  } else {
    flgcb->flgptn |= setptn;
    // Set all waiting    
    queue = flgcb->obj.waitq.next;
    while (queue != &(flgcb->obj.waitq)) {
      tcb = (TCB *) queue;
      queue = queue->next;
      wfmode = tcb->waitinfo.flg.wfmode;
      if (eventflag_cond(flgcb, tcb->waitinfo.flg.waiptn, wfmode)) {
        *(tcb->waitinfo.flg.p_flgptn) = flgcb->flgptn;
        wait_release_ok(tcb);
        if (flgcb->obj.objatr & TA_CLR)
          flgcb->flgptn = 0;
      }
    }
    err = E_OK;
  }
  END_CRITICAL_SECTION;
  return err;
}
//==============================================================================
ER iset_flg(ID flgid, FLGPTN setptn)
{
  return _set_flg(flgid, setptn);
}
//==============================================================================
ER set_flg(ID flgid, FLGPTN setptn)
{
  ER ercd;
  ercd = _set_flg(flgid, setptn);
  dispatch();
  return ercd;
}
//==============================================================================
ER clr_flg(ID flgid, FLGPTN clrptn)
{
  FLGCB *flgcb;
  
  if (flgid < TMIN_FLGID || flgid > TMAX_FLGID)
    return E_ID;
  
  flgcb = get_flgcb_by_id(flgid);
  
  if (TTS_NOEXS == flgcb->obj.state)
    return E_NOEXS;
   
  BEGIN_CRITICAL_SECTION;
  flgcb->flgptn &= clrptn;
  END_CRITICAL_SECTION;
  return E_OK;
}
//==============================================================================
static ER __wai_flg(ID flgid, FLGPTN waiptn, MODE wfmode,
                    FLGPTN *p_flgptn, TMO tmout)
{
  FLGCB *flgcb;
  
  if (flgid < TMIN_FLGID || flgid > TMAX_FLGID)
    return E_ID;
  
  if( ((wfmode!= TWF_ANDW)&&(wfmode!= TWF_ORW) ) || (0 == waiptn))
    return E_PAR;

  BEGIN_CRITICAL_SECTION;
  flgcb = get_flgcb_by_id(flgid);
  if (TTS_NOEXS == flgcb->obj.state) {
    runtsk->ercd = E_NOEXS;
  } else {
    if ((flgcb->obj.objatr & TA_WSGL) && !kqueue_empty(&(flgcb->obj.waitq))) {
       runtsk->ercd = E_ILUSE;
    } else {
      if (eventflag_cond(flgcb, waiptn, wfmode)) {
        runtsk->ercd = E_OK;
        *p_flgptn = flgcb->flgptn;
        clear_flgptn(flgcb);
      } else {
        runtsk->ercd = E_TMOUT;
        if (TMO_POL != tmout) {
          runtsk->waitinfo.flg.waiptn = waiptn;
          runtsk->waitinfo.flg.wfmode = wfmode;
          runtsk->waitinfo.flg.p_flgptn = p_flgptn;
          obj_make_wait(&(flgcb->obj), tmout);
          dispatch();
        }
      }
    }
  }
  END_CRITICAL_SECTION;
  
  return runtsk->ercd;
}
//------------------------------------------------------------------------------
ER wai_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn)
{
  return __wai_flg(flgid, waiptn, wfmode, p_flgptn, TMO_FEVR);
}
//------------------------------------------------------------------------------
ER twai_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn, TMO tmout)
{
  return __wai_flg(flgid, waiptn, wfmode, p_flgptn, tmout);
}
//------------------------------------------------------------------------------
ER pol_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn)
{
  return __wai_flg(flgid, waiptn, wfmode, p_flgptn, TMO_POL);
}
/*******************************************************************************
 Reference Eventflag Status
 *******************************************************************************/
ER ref_flg(ID flgid, T_RFLG *pk_rflg)
{
  FLGCB *flgcb;
  ER err;

  if (flgid < TMIN_FLGID || flgid > TMAX_FLGID)
    return E_ID;
  
  flgcb = get_flgcb_by_id(flgid);
  
  BEGIN_CRITICAL_SECTION;
  if (TTS_NOEXS == flgcb->obj.state) {
    err = E_NOEXS;
  } else {
    pk_rflg->wtskid = flgcb->obj.objid;
    pk_rflg->flgptn = flgcb->flgptn;
    err = E_OK;
  }
  END_CRITICAL_SECTION;

  return err;
}

#endif // USE_EVENTFLAG
