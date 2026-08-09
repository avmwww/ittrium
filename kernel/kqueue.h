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

static inline void kqueue_init(KQUEUE *queue)
{
  queue->prev = queue->next = queue;
}

static inline void kqueue_insert(KQUEUE *entry, KQUEUE *queue)
{
  entry->prev = queue->prev;
  entry->next = queue;
  queue->prev->next = entry;
  queue->prev = entry;
}

static inline void kqueue_remove(KQUEUE *entry)
{
  if (entry->next != entry) {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
  }
}

static inline KQUEUE *kqueue_remove_next(KQUEUE *queue)
{
  KQUEUE *entry;

  entry = queue->next;
  queue->next = entry->next;
  entry->next->prev = queue;
  return(entry);
}
/*
static KQUEUE *queue_search_gt(KQUEUE *queue, INT val, INT offset)
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

static inline BOOL kqueue_empty(KQUEUE *queue)
{
  if (queue->next == queue) return(TRUE);
  else return(FALSE);
}


#endif /* _KQUEUE_H_ */
