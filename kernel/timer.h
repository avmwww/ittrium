/******************************************************************************
**	Filename:		timer.h
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
#ifndef _TIMER_H_
#define _TIMER_H_
#include "ittrium.h"
#include "queue.h"

typedef struct timer_event_block {
  QUEUE    queue;
  RELTIM   count;
  FP_VP    callback;
  VP       arg;
} TMEB;

extern void start_hw_timer();
extern void terminate_hw_timer();

extern void timer_shutdown(void);

extern void timer_insert(TMEB *event, TMO tmout, FP_VP callback, VP arg);
extern void timer_insert_abs(TMEB *event, SYSTIM *time, FP_VP callback, VP arg);
extern void timer_insert_rel(TMEB *event, SYSTIM *time, FP_VP callback, VP arg);

/**
 * Delete timer event from timer event queue
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void timer_delete(TMEB *event)
{
  queue_delete(&(event->queue));
}

#endif /* _TIMER_H_ */
