/******************************************************************************
**	Filename:		timer.c
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
#include "queue.h"
#include "timer.h"

static QUEUE	timer_queue;

void timer_initialize(void)
{
  queue_initialize(&timer_queue);
  start_hw_timer();
}

void timer_shutdown(void)
{
   terminate_hw_timer();
}

static void enqueue_tmeb(TMEB *event)
{
   queue_insert(&(event->queue), &timer_queue);
}

void timer_insert(TMEB *event, TMO tmout, FP_VP callback, VP arg)
{
   event->callback = callback;
   event->arg = arg;

   if (tmout == TMO_FEVR)
   {
      queue_initialize(&(event->queue));
   }
   else
   {
      event->count = tmout;
      enqueue_tmeb(event);
   }
}

// timer_hw_enter => return nesting count, if timer nesting, don't run handler
extern int timer_hw_enter(void);
// timer_hw_exit => clear nesting counter
extern void timer_hw_exit(void);

void timer_handler(void)
{
  TMEB *event = (TMEB *)(timer_queue.next);
  TMEB *next;

  if (timer_hw_enter()) return;

  BEGIN_CRITICAL_SECTION;
  while (event != (TMEB *)&timer_queue) {
    next = (TMEB *)(event->queue.next);
    if (0 == --event->count) {
      queue_delete(&(event->queue));
      if (event->callback)
        (*(event->callback))(event->arg);
    }
    event = next;
  }
  END_CRITICAL_SECTION;
  timer_hw_exit();
}

