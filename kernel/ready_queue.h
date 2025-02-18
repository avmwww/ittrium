/******************************************************************************
**	Filename:		ready_queue.h
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
#ifndef _READY_QUEUE_H_
#define _READY_QUEUE_H_

#include "ittrium.h"
#include "queue.h"
#include "task.h"

#define BITMAPSZ	(1<<BITMAPSHIFT)
#define NUM_BITMAP	((TNUM_TPRI + BITMAPSZ - 1) / BITMAPSZ)

typedef	struct ready_queue {
  PRI     top_priority;        /**< ��������� ��������� � �������  */
  QUEUE   tskque[TMAX_TPRI];   /**< ������� ����� � ��������� READY */
  TCB    *null;
  UINT    bitmap[NUM_BITMAP];  /**< ���� �������� ����������� */
} RDYQUE;


/**
 * � ������� ������� ���� ������������� ������� ����������,
 * ���� ��� ���������� (=1), �� ����� ������� ���������� �������,
 * ���� ������� (=0), �� ����� ����� ���������� �� ��������������.
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif // INLINE_PRAGMA
void bitmap_set(RDYQUE *rq, PRI priority)
{
  rq->bitmap[priority >>BITMAPSHIFT] |= 1 << (priority & (BITMAPSZ-1));
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif // INLINE_PRAGMA
void bitmap_clear(RDYQUE *rq, INT priority)
{
  rq->bitmap[priority >>BITMAPSHIFT] &= ~(1 << (priority & (BITMAPSZ-1)));
}

/**
 * ���� � ������� � ��������� ���� ����� ������ 1, ��� �������������
 * ������� ����������. ���������� ������� ����� ����(����� ����������).
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif // INLINE_PRAGMA
UINT _ffs(UINT i)
{
  UINT j = 0;

  while ((i & 1) == 0) {
    i >>= 1;
    j++;
  }
  return(j);
}

/**
 * ������������� ������������ �����
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void ready_queue_initialize(RDYQUE *rq)
{
  INT i;

  rq->top_priority = TMAX_TPRI;
  for (i = 0; i < TMAX_TPRI; i++)
     queue_initialize(&rq->tskque[i]);

  rq->null = (TCB *) 0;
  for (i=0; i < NUM_BITMAP; i++)
    rq->bitmap[i] = 0;
}

/**
 * ���������� TCB ������������ � �������, ������� ������ ���������.
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
TCB *ready_queue_top(RDYQUE *rq)
{
  return((TCB *)(rq->tskque[rq->top_priority].next));
}

/**
 * ���������� �������� ������� ����������.
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
PRI ready_queue_top_priority(RDYQUE *rq)
{
  return(rq->top_priority);
}

/**
 * ��������� TCB � ����� ������� �� ��������������� ��� ����� ����������.
 * �������� ������ ������� ���������.
 * ���� ��������� TCB ���� ��������, �� ���������� TRUE.
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
BOOL ready_queue_insert(RDYQUE *rq, TCB *tcb)
{
  PRI priority = tcb->tskpri;

  queue_insert(&(tcb->tskque), &(rq->tskque[priority]));
  bitmap_set(rq, priority);
  if (priority < rq->top_priority) {
    rq->top_priority = priority;
    return(TRUE);
  }
  return(FALSE);
}

/**
 * ��������� TCB � ������ ������� �� ��������������� ��� ����� ����������.
 * �������� ������ ������� ���������.
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void ready_queue_insert_top(RDYQUE *rq, TCB *tcb)
{
  INT priority = tcb->tskpri;

  queue_insert(&(tcb->tskque), rq->tskque[priority].next);
  bitmap_set(rq, priority);
  if (priority < rq->top_priority)
    rq->top_priority = priority;
}

/**
 * ������� TCB �� ������� �� ��������������� ��� ����� ����������.
 * �������� ������ ������� ���������.
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void ready_queue_delete(RDYQUE *rq, TCB *tcb)
{
  PRI priority = tcb->tskpri;
  INT i;

  queue_delete(&(tcb->tskque));
  
  if (!queue_is_empty(&(rq->tskque[priority])))
    return;
  /*
   * ���� �� ������ ����� ����������� ������ ������ � ������� ���,
   * �� ������� ����.
   */
  bitmap_clear(rq, priority);
  if (priority != rq->top_priority)
    return;
  /*
   * ������������� ������� ������ ���������
   */
  for (i = (priority >> BITMAPSHIFT); i < NUM_BITMAP; i++) {
    if (rq->bitmap[i]) {
      rq->top_priority = (i << BITMAPSHIFT) + _ffs(rq->bitmap[i]);
      return;
    }
  }
  /*
   * ���� ����� ����������� �� �������, ������ ������ ��������� ����.
   */
  rq->top_priority = TMAX_TPRI;
}

/**
 * �������� �� ���� �� ������ ������� �� �������� ����� ����������.
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void ready_queue_rotate(RDYQUE *rq, INT priority)
{
  QUEUE   *entry, *queue;

  queue = &(rq->tskque[priority]);
  if (!queue_is_empty(queue)) {
    entry = queue_delete_next(queue);
    queue_insert(entry, queue);
  }
}

#endif /* _READY_QUEUE_H_ */
