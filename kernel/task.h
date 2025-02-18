/******************************************************************************
**	Filename:		task.h
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
#ifndef _TASK_H_
#define _TASK_H_


#include "ittrium.h"
#include "queue.h"
#include "timer.h"
#include "winfo.h"

typedef struct task_control_block		TCB;
typedef struct generic_control_block	GCB;


#define int_priority(x)	((INT)((x) - TMIN_TPRI))
#define ext_tskpri(x)	((PRI)((x) + TMIN_TPRI))

struct generic_control_block {
  QUEUE   wait_queue;
  ID      objid;
  VP      exinf;
  ATR     objatr;
  STAT    state;
};

struct task_control_block {
  QUEUE   tskque;
  CTXB    tskctxb;
  FP      task;      // Task start address
  VP      stk;       // Base address of task stack space
#ifdef USE_SEPARATE_STACK
  VP      sstk;      // Base address of task system stack space	
#endif // USE_SEPARATE_STACK
  SIZE    stksz;     // Task stack size (in bytes)
  ID      tskid;
  VP_INT  exinf;     // Task extend information
  ATR     tskatr;    // Task attribute
  PRI     itskpri;   // Task initial priority
  PRI     tskpri;    // Task current priority
  INT     actcnt;    // Activate request count
  STAT    state;
  GCB     *wgcb;
  WINFO   winfo;     // Wait info
  ER       ercd;
  INT     wupcnt;
  INT     suscnt;
#ifdef USE_MAILBOX
  T_MSG   *tmq_head;
  T_MSG   *tmq_tail;
#endif /* USE_MAILBOX */
  ER     *wercd;
  TMEB    wtmeb;
};

extern TCB	*runtsk;
extern TCB	*schedtsk;

extern ID 	current_tsk_id;
extern ID 	shed_tsk_id;

extern TCB		tcb_table[];
extern QUEUE	free_tcb;

#define get_tcb(id)   (&tcb_table[(id) - TMIN_TSKID])

#define get_tcb_self(id) ((id)==TSK_SELF ? runtsk : &tcb_table[(id) - TMIN_TSKID])

extern void make_dormant(TCB *tcb);
extern void move_to_ready_state(TCB *tcb);
extern void move_from_ready_state(TCB *tcb);
extern void change_task_priority(TCB *tcb, PRI priority);
extern void rotate_ready_queue(PRI priority);
extern void rotate_ready_queue_run(void);
extern void make_task_context(TCB *tcb);

#endif /* _TASK_H_ */
