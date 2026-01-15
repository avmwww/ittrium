/******************************************************************************
**	Filename:		main.c
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

void task_initialize(void);
void timer_initialize(void);
#ifdef USE_EVENTFLAG
void eventflag_initialize(void);
#endif // USE_EVENTFLAG

#ifdef USE_SEMAPHORE
void semaphore_initialize(void);
#endif // USE_SEMAPHORE

#ifdef USE_MAILBOX
void mailbox_initialize(void);
#endif //USE_MAILBOX

int main(int argc, char **argv)
{
  T_CTSK pk_ctsk;

  low_level_init();

  task_initialize();
  timer_initialize();
#ifdef USE_EVENTFLAG
  eventflag_initialize();
#endif // USE_EVENTFLAG

#ifdef USE_SEMAPHORE
  semaphore_initialize();
#endif // USE_SEMAPHORE

#ifdef USE_MAILBOX
 mailbox_initialize();
#endif //USE_MAILBOX
  //***************************************************************************
  // Start INIT task
  //***************************************************************************
  pk_ctsk.tskatr = INIT_TASK_ATTR;
  pk_ctsk.exinf = INIT_TASK_EXINF;
  pk_ctsk.task = (FP)INIT_TASK;
  pk_ctsk.itskpri = INIT_TASK_PRI;
  pk_ctsk.stksz = INIT_TASK_STACK_SIZE;
  pk_ctsk.stk = INIT_TASK_STACK;
#ifdef USE_SEPARATE_STACK
  pk_ctsk.sstk = INIT_TASK_SYS_STACK;
#endif // USE_SEPARATE_STACK

  cre_tsk(INIT_TASK_ID, &pk_ctsk);
  act_tsk(INIT_TASK_ID);
  // We never arrive here
  for(;;);
}
