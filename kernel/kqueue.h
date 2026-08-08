/* ittrium kernel — kqueue.h
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef	_KQUEUE_H_
#define	_KQUEUE_H_

typedef struct kqueue {
  struct kqueue *next;
  struct kqueue *prev;
} KQUEUE;

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void kqueue_init(KQUEUE *queue)
{
  queue->prev = queue->next = queue;
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void kqueue_insert(KQUEUE *entry, KQUEUE *queue)
{
  entry->prev = queue->prev;
  entry->next = queue;
  queue->prev->next = entry;
  queue->prev = entry;
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
void kqueue_remove(KQUEUE *entry)
{
  if (entry->next != entry) {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
  }
}

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
KQUEUE *kqueue_remove_next(KQUEUE *queue)
{
  KQUEUE *entry;

  entry = queue->next;
  queue->next = entry->next;
  entry->next->prev = queue;
  return(entry);
}
/*
INLINE KQUEUE *queue_search_gt(KQUEUE *queue, INT val, INT offset)
{
  KQUEUE *entry;

  for (entry = queue->next; entry != queue; entry = entry->next) {
    if (*((INT *)(((VB *) entry) + offset)) > val) {
      break;
    }
  }
  return(entry);
}
*/

#ifdef INLINE_PRAGMA
#pragma inline
#else
INLINE
#endif
BOOL kqueue_empty(KQUEUE *queue)
{
  if (queue->next == queue) return(TRUE);
  else return(FALSE);
}


#endif /* _KQUEUE_H_ */
