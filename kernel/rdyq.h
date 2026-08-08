/* ittrium kernel — rdyq.h
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef _RDYQ_H_
#define _RDYQ_H_

#include "ittrium.h"
#include "kqueue.h"
#include "task.h"

#define BITMAPSZ	(1<<BITMAPSHIFT)
#define NUM_BITMAP	((TNUM_TPRI + BITMAPSZ - 1) / BITMAPSZ)

typedef	struct rdyq {
  PRI     top_priority;
  KQUEUE  priq[TMAX_TPRI];
  TCB    *null;
  UINT    bitmap[NUM_BITMAP];
} RDYQ;

/* Each bitmap bit marks a non-empty priority line. */

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void rdyq_bitmap_set(RDYQ *rq, PRI priority)
{
  rq->bitmap[priority >>BITMAPSHIFT] |= 1 << (priority & (BITMAPSZ-1));
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void rdyq_bitmap_clear(RDYQ *rq, INT priority)
{
  rq->bitmap[priority >>BITMAPSHIFT] &= ~(1 << (priority & (BITMAPSZ-1)));
}

/* Lowest set bit index (= highest µITRON priority on that word). */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
UINT _ffs(UINT i)
{
  UINT j = 0;

  while ((i & 1) == 0) {
    i >>= 1;
    j++;
  }
  return(j);
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void rdyq_init(RDYQ *rq)
{
  INT i;

  rq->top_priority = TMAX_TPRI;
  for (i = 0; i < TMAX_TPRI; i++)
     kqueue_init(&rq->priq[i]);

  rq->null = (TCB *) 0;
  for (i=0; i < NUM_BITMAP; i++)
    rq->bitmap[i] = 0;
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
TCB *rdyq_top(RDYQ *rq)
{
  return((TCB *)(rq->priq[rq->top_priority].next));
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
PRI rdyq_top_pri(RDYQ *rq)
{
  return(rq->top_priority);
}

/* Enqueue at end of the task's priority line. TRUE if it becomes top. */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
BOOL rdyq_insert(RDYQ *rq, TCB *tcb)
{
  PRI priority = tcb->tskpri;

  kqueue_insert(&(tcb->qnode), &(rq->priq[priority]));
  rdyq_bitmap_set(rq, priority);
  if (priority < rq->top_priority) {
    rq->top_priority = priority;
    return(TRUE);
  }
  return(FALSE);
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void rdyq_insert_front(RDYQ *rq, TCB *tcb)
{
  INT priority = tcb->tskpri;

  kqueue_insert(&(tcb->qnode), rq->priq[priority].next);
  rdyq_bitmap_set(rq, priority);
  if (priority < rq->top_priority)
    rq->top_priority = priority;
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void rdyq_remove(RDYQ *rq, TCB *tcb)
{
  PRI priority = tcb->tskpri;
  INT i;

  kqueue_remove(&(tcb->qnode));

  if (!kqueue_empty(&(rq->priq[priority])))
    return;

  rdyq_bitmap_clear(rq, priority);
  if (priority != rq->top_priority)
    return;

  for (i = (priority >> BITMAPSHIFT); i < NUM_BITMAP; i++) {
    if (rq->bitmap[i]) {
      rq->top_priority = (i << BITMAPSHIFT) + _ffs(rq->bitmap[i]);
      return;
    }
  }
  rq->top_priority = TMAX_TPRI;
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void rdyq_rotate(RDYQ *rq, INT priority)
{
  KQUEUE   *entry, *queue;

  queue = &(rq->priq[priority]);
  if (!kqueue_empty(queue)) {
    entry = kqueue_remove_next(queue);
    kqueue_insert(entry, queue);
  }
}

#endif /* _RDYQ_H_ */
