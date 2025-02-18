#ifndef _SERVICE_CALL_H_
#define _SERVICE_CALL_H_

/** Task Management Functions */

/** Create Task
 */
ER cre_tsk(ID tskid, T_CTSK *pk_ctsk);

/** Create Task (ID Number Automatic Assignment)
 */
ER_ID acre_tsk(T_CTSK *pk_ctsk);

/** Activate Task
 */
ER act_tsk(ID tskid);

/** Activate Task from interrupt context
 */
ER iact_tsk(ID tskid);

/**
 * Terminate Invoking Task
 */
void ext_tsk(void);

/** Change Task Priority
 */
ER chg_pri(ID tskid, PRI tskpri);

/** Mailbox */

/**
 */
ER cre_mbx ( ID mbxid, T_CMBX *pk_cmbx );

/**
 */
ER snd_mbx (ID mbxid, T_MSG *pk_msg);

/**
 */
ER rcv_mbx (ID mbxid, T_MSG **ppk_msg);

/**
 */
ER prcv_mbx (ID mbxid, T_MSG **ppk_msg);

/**
 */
ER trcv_mbx (ID mbxid, T_MSG **ppk_msg, TMO tmout);

/** Task Dependent Synchronization Functions */

/** Suspend Task
 */
ER sus_tsk(ID tskid);

/** Resume Suspended Task
 */
ER rsm_tsk(ID tskid);

/** Delay Task
 */
ER dly_tsk(RELTIM dlytim);

/** Synchronization and Communication Functions */

/** Semaphores */

/** Create Semaphore
 */
ER cre_sem(ID semid, T_CSEM *pk_csem);

/** Acquire Semaphore Resource
 */
ER wai_sem(ID semid);

/** Acquire Semaphore Resource (with Timeout)
 */
ER twai_sem(ID semid, TMO tmout);

/** Acquire Semaphore Resource (Polling)
 */
ER pol_sem(ID semid);

/** Release Semaphore Resource
 */
ER sig_sem(ID semid);

/** Eventflags */

/** Create Eventflag
 */
ER cre_flg(ID flgid, T_CFLG *pk_cflg);

/** Create Eventflag (ID Number Automatic Assignment)
 */
ER_ID acre_flg(T_CFLG *pk_cflg);

/** Set Eventflag
 */
ER set_flg(ID flgid, FLGPTN setptn);

/** Set Eventflag from interrupt context
 */
ER iset_flg(ID flgid, FLGPTN setptn);

/** Wait for Eventflag
 */
ER wai_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn);

/** Wait for Eventflag (Polling)
 */
ER pol_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn);

/** Wait for Eventflag (with Timeout)
 */
ER twai_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn, TMO tmout);

/** Reference Eventflag Status
 */
ER ref_flg(ID flgid, T_RFLG *pk_rflg);

/** Clear Eventflag
 */
ER clr_flg(ID flgid, FLGPTN clrptn);

/** Time Management Functions */

/** System Time Management */

/** Set System Time
 */
ER set_tim(SYSTIM *p_systim);

/** Reference System Time
 */
ER get_tim(SYSTIM *p_systim);



#endif // _SERVICE_CALL_H_

