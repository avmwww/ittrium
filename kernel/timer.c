/* ittrium kernel — timer.c
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "ittrium.h"
#include "kqueue.h"
#include "timer.h"
#include "task.h"

static KQUEUE	timer_queue;

void timer_initialize(void)
{
  kqueue_init(&timer_queue);
  start_hw_timer();
}

void timer_shutdown(void)
{
   terminate_hw_timer();
}

static void enqueue_tmeb(TMEB *event)
{
   kqueue_insert(&(event->queue), &timer_queue);
}

void timer_insert(TMEB *event, TMO tmout, FP_VP callback, VP arg)
{
   event->callback = callback;
   event->arg = arg;

   if (tmout == TMO_FEVR)
   {
      kqueue_init(&(event->queue));
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

  telemetry_wall_ticks++;
  if (runtsk)
    runtsk->run_ticks++;
  else
    telemetry_idle_ticks++;

  BEGIN_CRITICAL_SECTION;
  while (event != (TMEB *)&timer_queue) {
    next = (TMEB *)(event->queue.next);
    if (0 == --event->count) {
      kqueue_remove(&(event->queue));
      if (event->callback)
        (*(event->callback))(event->arg);
    }
    event = next;
  }
  END_CRITICAL_SECTION;
  timer_hw_exit();
}

