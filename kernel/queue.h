/******************************************************************************
**	Filename:		queue.h
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
#ifndef	_QUEUE_H_
#define	_QUEUE_H_

typedef struct queue {
  struct queue *next;
  struct queue *prev;
} QUEUE;

/**
 *  Queue initialization
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void queue_initialize(QUEUE *queue)
{
  queue->prev = queue->next = queue;
}

/**
 * Insert entry to queue
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void queue_insert(QUEUE *entry, QUEUE *queue)
{
  entry->prev = queue->prev;
  entry->next = queue;
  queue->prev->next = entry;
  queue->prev = entry;
}

/**
 * Remove entry from queue
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void queue_delete(QUEUE *entry)
{
  if (entry->next != entry) {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
  }
}

/**
 * Remove next entry (first in queue) from queue
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
QUEUE *queue_delete_next(QUEUE *queue)
{
  QUEUE *entry;

  entry = queue->next;
  queue->next = entry->next;
  entry->next->prev = queue;
  return(entry);
}
/*
INLINE QUEUE *queue_search_gt(QUEUE *queue, INT val, INT offset)
{
  QUEUE *entry;

  for (entry = queue->next; entry != queue; entry = entry->next) {
    if (*((INT *)(((VB *) entry) + offset)) > val) {
      break;
    }
  }
  return(entry);
}
*/

/**
 * Check to queue is empty
 */
#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
BOOL queue_is_empty(QUEUE *queue)
{
  if (queue->next == queue) return(TRUE);
  else return(FALSE);
}


#endif /* _QUEUE_H_ */
