/******************************************************************************
**	Filename:		mailbox.c
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
#ifdef USE_MAILBOX

//Check mailbox ID
#define   CHECK_MBX_ID(mbxid)    \
  if (mbxid < TMIN_MBXID || mbxid > TMAX_MBXID)  \
    return E_ID;

//Check timeout
#define CHECK_TMOUT(tmout)       \
  if (!((tmout) >= TMO_FEVR))    \
    return E_PAR;

/*
 * Mailbox control block
 *
 *'mq_head' is the first message queue pointer that points a message.
 * It is NULL if the message queue is empty.
 * 'mq_tail' is a pointer that points end of message queue that is not empty.
 * The message queue value is not guaranteed if the message queue is empty.
 * It is used only if the message queue is FIFO (TA_MFIFO).
 */
typedef struct mailbox_control_block {
  GCB    gcb;
  T_MSG  mq_head; // Head of message queue 
  T_MSG *mq_tail; // End of message queue 
} MBXCB;

static MBXCB mbxcb_table[TNUM_MBXID]; // Mailbox control block 

#define get_mbxcb_by_id(id)  (&(mbxcb_table[(id) - TMIN_MBXID]))

/*******************************************************************************
 Startup initialize
 *******************************************************************************/
void mailbox_initialize()
{
  ID    id;
  MBXCB *mbxcb;

  for(mbxcb = mbxcb_table, id = 0; id  < TNUM_MBXID; mbxcb++, id++) 
  {
    mbxcb->gcb.objid = id + TMIN_MBXID;
    mbxcb->gcb.state = TTS_NOEXS;
    queue_initialize(&(mbxcb->gcb.wait_queue));
  }
}

/*
 * Head message
 */
#define headmsg(mbxcb)  ( (mbxcb)->mq_head.msgque[0] )

/*
 * Next message
 */
#define nextmsg(msg)    ( ((T_MSG*)(msg))->msgque[0] )

/*
 * Insert a message queue following priority
 */
INLINE void queue_insert_mpri( T_MSG_PRI *pk_msg, T_MSG *head )
{
  T_MSG_PRI *msg;
  T_MSG *prevmsg = head;

  while ( (msg = (T_MSG_PRI*)nextmsg(prevmsg)) != NULL ) {
    if ( msg->msgpri > pk_msg->msgpri ) {
      break;
    }
    prevmsg = (T_MSG*)msg;
  }
  nextmsg(pk_msg) = msg;
  nextmsg(prevmsg) = pk_msg;
}

/*
 * Create mailbox
 */
ER cre_mbx(ID mbxid, T_CMBX *pk_cmbx)
{
  MBXCB *mbxcb;
  ER ercd;

  CHECK_MBX_ID(mbxid)

    if (pk_cmbx->mbxatr !=
        (pk_cmbx->mbxatr & ((TA_TFIFO|TA_TPRI) | (TA_MFIFO|TA_MPRI))))
      return E_RSATR;

  mbxcb = get_mbxcb_by_id(mbxid);

  BEGIN_CRITICAL_SECTION;
  if (TTS_NOEXS != mbxcb->gcb.state) {
    ercd = E_OBJ;
  } else {
    queue_initialize(&(mbxcb->gcb.wait_queue));

    mbxcb->gcb.objatr = pk_cmbx->mbxatr;
    mbxcb->gcb.state  = TTS_RDY;
    mbxcb->mq_head.msgque[0] = NULL;

    ercd = E_OK;
  }

  END_CRITICAL_SECTION;

  return ercd;
}
/*******************************************************************************
  This service call sends the message whose start address is specified by
 pk_msg to the mailbox specified by mbxid
 *******************************************************************************/
ER __snd_mbx(ID mbxid, T_MSG *pk_msg)
{
  MBXCB *mbxcb;
  TCB   *tcb;
  ER    ercd = E_OK;

  CHECK_MBX_ID(mbxid);

  mbxcb = get_mbxcb_by_id(mbxid);

  BEGIN_CRITICAL_SECTION;
  if (TTS_NOEXS == mbxcb->gcb.state) {
    // Non-existent object (specified mailbox is not registered)
    ercd = E_NOEXS;
    goto error_exit;
  } 

  if (mbxcb->gcb.objatr & TA_MPRI) {
    if ( ((T_MSG_PRI*)pk_msg)->msgpri <= 0 ) {
      ercd = E_PAR;
      goto error_exit;
    }
  }

  if ( !queue_is_empty( &(mbxcb->gcb.wait_queue) ) ){
    //Directly send to receive wait task 
    tcb = (TCB*)(mbxcb->gcb.wait_queue.next);
    *(tcb->winfo.mbx.ppk_msg) = pk_msg;
    wait_release_ok(tcb);

  }else{
    //Connect message to queue
    if ((mbxcb->gcb.objatr & TA_MPRI) != 0){
      //Connect message to queue following priority 
      queue_insert_mpri((T_MSG_PRI*)pk_msg, &mbxcb->mq_head); 
    } else {
      //Connect to end of queue
      nextmsg(pk_msg) = NULL;
      if ( headmsg(mbxcb) == NULL ) {
        headmsg(mbxcb) = pk_msg;
      } else {
        nextmsg(mbxcb->mq_tail) = pk_msg;
      }
      mbxcb->mq_tail = pk_msg;
    }
  }

error_exit:
  END_CRITICAL_SECTION;

  return ercd;
}
///////////////////////////////////////////////////////////////////
//==============================================================================
ER snd_mbx(ID mbxid, T_MSG *pk_msg)
{
  ER ercd;
  ercd = __snd_mbx(mbxid, pk_msg);
  dispatch();
  return ercd;
}
//==============================================================================
/*
 * Receive from mailbox
 */
ER __rcv_mbx(ID mbxid, T_MSG **ppk_msg, TMO tmout)
{
  MBXCB *mbxcb;

  CHECK_MBX_ID(mbxid);
  CHECK_TMOUT(tmout);

  mbxcb = get_mbxcb_by_id(mbxid);

  BEGIN_CRITICAL_SECTION;

  if (TTS_NOEXS == mbxcb->gcb.state) {
    // Non-existent object (specified mailbox is not registered)
    runtsk->ercd = E_NOEXS;
  } else {
    if ( headmsg(mbxcb) != NULL ) {
      // Get message from head of queue 
      runtsk->ercd = E_OK;
      *ppk_msg = headmsg(mbxcb);
      headmsg(mbxcb) = nextmsg(*ppk_msg);
    } else {
      //Ready for receive wait
      runtsk->ercd = E_TMOUT;
      if (tmout != TMO_POL) {
        runtsk->winfo.mbx.ppk_msg = ppk_msg;
        gcb_make_wait(&(mbxcb->gcb), tmout);
        dispatch();
      }
    }
  }

  END_CRITICAL_SECTION;

  return runtsk->ercd;
}

/*
 * Receive from mailbox
 */
ER rcv_mbx(ID mbxid, T_MSG **ppk_msg)
{
  return __rcv_mbx(mbxid, ppk_msg, TMO_FEVR);
}
/*
 * Receive from mailbox polling
 */
ER prcv_mbx(ID mbxid, T_MSG **ppk_msg)
{
  return __rcv_mbx(mbxid, ppk_msg, TMO_POL);
}
/*
 * Receive from mailbox polling
 */
ER trcv_mbx(ID mbxid, T_MSG **ppk_msg, TMO tmout)
{
  return __rcv_mbx(mbxid, ppk_msg, tmout);
}

/*
 * Refer mailbox state 
 */
ER ref_mbx( ID mbxid, T_RMBX *pk_rmbx )
{
  MBXCB *mbxcb;
  ER ercd = E_OK;

  CHECK_MBX_ID(mbxid);

  mbxcb = get_mbxcb_by_id(mbxid);

  BEGIN_CRITICAL_SECTION;
  if (TTS_NOEXS == mbxcb->gcb.state) {
    ercd = E_NOEXS;
  } else {
    pk_rmbx->wtskid = mbxcb->gcb.objid;
    pk_rmbx->pk_msg = headmsg(mbxcb);
  }
  END_CRITICAL_SECTION;

  return ercd;
}
 
#endif // USE_MAILBOX
