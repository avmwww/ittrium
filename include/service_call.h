#ifndef _SERVICE_CALL_H_
#define _SERVICE_CALL_H_

/* Task Management */

ER cre_tsk(ID tskid, T_CTSK *pk_ctsk);
ER_ID acre_tsk(T_CTSK *pk_ctsk);
ER act_tsk(ID tskid);
ER iact_tsk(ID tskid);
void ext_tsk(void);
ER del_tsk(ID tskid);
ER chg_pri(ID tskid, PRI tskpri);
ER rot_rdq(PRI tskpri);

/* Mailbox */

ER cre_mbx(ID mbxid, T_CMBX *pk_cmbx);
ER_ID acre_mbx(T_CMBX *pk_cmbx);
ER del_mbx(ID mbxid);
ER snd_mbx(ID mbxid, T_MSG *pk_msg);
ER rcv_mbx(ID mbxid, T_MSG **ppk_msg);
ER prcv_mbx(ID mbxid, T_MSG **ppk_msg);
ER trcv_mbx(ID mbxid, T_MSG **ppk_msg, TMO tmout);
ER ref_mbx(ID mbxid, T_RMBX *pk_rmbx);

/* Task Dependent Synchronization */

ER sus_tsk(ID tskid);
ER rsm_tsk(ID tskid);
ER dly_tsk(RELTIM dlytim);
ER slp_tsk(void);
ER tslp_tsk(TMO tmout);
ER wup_tsk(ID tskid);
ER iwup_tsk(ID tskid);
ER can_wup(ID tskid, INT *p_wupcnt);

/* Semaphores */

ER cre_sem(ID semid, T_CSEM *pk_csem);
ER_ID acre_sem(T_CSEM *pk_csem);
ER del_sem(ID semid);
ER wai_sem(ID semid);
ER twai_sem(ID semid, TMO tmout);
ER pol_sem(ID semid);
ER sig_sem(ID semid);
ER isig_sem(ID semid);
ER ref_sem(ID semid, T_RSEM *pk_rsem);

/* Eventflags */

ER cre_flg(ID flgid, T_CFLG *pk_cflg);
ER_ID acre_flg(T_CFLG *pk_cflg);
ER del_flg(ID flgid);
ER set_flg(ID flgid, FLGPTN setptn);
ER iset_flg(ID flgid, FLGPTN setptn);
ER wai_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn);
ER pol_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn);
ER twai_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn, TMO tmout);
ER ref_flg(ID flgid, T_RFLG *pk_rflg);
ER clr_flg(ID flgid, FLGPTN clrptn);

/* Time Management */

ER set_tim(SYSTIM *p_systim);
ER get_tim(SYSTIM *p_systim);

#endif /* _SERVICE_CALL_H_ */
