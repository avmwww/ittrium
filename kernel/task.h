/* ittrium kernel — task.h
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef _TASK_H_
#define _TASK_H_


#include "ittrium.h"
#include "kqueue.h"
#include "timer.h"
#include "waitinfo.h"

typedef struct task_control_block		TCB;
typedef struct object_control_block	OBJCB;


#define pri_to_index(x)	((INT)((x) - TMIN_TPRI))
#define index_to_pri(x)	((PRI)((x) + TMIN_TPRI))

struct object_control_block {
  KQUEUE   waitq;
  ID      objid;
  VP      exinf;
  ATR     objatr;
  STAT    state;
};

struct task_control_block {
  KQUEUE   qnode;
  CTXB    tskctxb;
  FP      task;      // Task start address
  VP      stk;       // Base address of task stack space
#ifdef USE_SEPARATE_STACK
  VP      sstk;      // Base address of task system stack space	
#endif // USE_SEPARATE_STACK
  SIZE    stksz;     // Task stack size (in bytes)
  VP      stk_base;  // Low address of stack (for canary / HWM)
  UW      run_ticks; // Scheduler ticks while this task was runtsk
  ID      tskid;
  VP_INT  exinf;     // Task extend information
  ATR     tskatr;    // Task attribute
  char    name[TSK_NAME_LEN]; /* copy of T_CTSK.name when TA_NAME */
  PRI     itskpri;   // Task initial priority
  PRI     tskpri;    // Task current priority
  INT     actcnt;    // Activate request count
  STAT    state;
  OBJCB     *wait_obj;
  WAITINFO   waitinfo;
  ER       ercd;
  UINT    tskwait;
  INT     wupcnt;
  INT     suscnt;
#ifdef USE_MAILBOX
  T_MSG   *tmq_head;
  T_MSG   *tmq_tail;
#endif /* USE_MAILBOX */
  ER     *wercd;
  TMEB    wtmeb;
};

extern volatile UW telemetry_wall_ticks;
extern volatile UW telemetry_idle_ticks;

extern TCB	*runtsk;
extern TCB	*schedtsk;

extern ID 	current_tsk_id;
extern ID 	shed_tsk_id;

extern TCB		tcb_table[];
extern KQUEUE	free_tcb;

#define get_tcb(id)   (&tcb_table[(id) - TMIN_TSKID])

#define get_tcb_self(id) ((id)==TSK_SELF ? runtsk : &tcb_table[(id) - TMIN_TSKID])

extern void task_make_dormant(TCB *tcb);
extern void task_make_ready(TCB *tcb);
extern void task_make_unready(TCB *tcb);
extern void task_change_pri(TCB *tcb, PRI priority);
extern void rdyq_rotate_at(PRI priority);
extern void rdyq_rotate_current(void);
extern void make_task_context(TCB *tcb);

#endif /* _TASK_H_ */
