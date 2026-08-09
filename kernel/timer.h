/* ittrium kernel — timer.h
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef _TIMER_H_
#define _TIMER_H_
#include "ittrium.h"
#include "kqueue.h"

typedef struct timer_event_block {
  KQUEUE    queue;
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
static inline void timer_delete(TMEB *event)
{
  kqueue_remove(&(event->queue));
}

#endif /* _TIMER_H_ */
