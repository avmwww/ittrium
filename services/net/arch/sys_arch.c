/**
 * lwIP sys_arch port for ittrium (μITRON)
 *
 * Semaphores: static ID pool (LWIP_SEM_ID_BASE.. or LWIP_SEM_IDS)
 * Threads:    pool of LWIP_THREAD_COUNT tasks with static stacks
 * Mailboxes:  ring buffer of void* + wait semaphores (not ittrium mbx)
 * Time:       get_tim() / 1 ms system ticks
 * Protect:    SYS_ARCH_PROTECT macros in sys_arch.h (BEGIN_CRITICAL_SECTION)
 */
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/stats.h"

#include "ittrium.h"

#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
/* Semaphore ID pool                                                          */
/* -------------------------------------------------------------------------- */

struct sys_sem {
  ID id;
  int slot; /* index in sem_pool, or -1 if heap-wrapped */
};

struct sem_slot {
  ID id;
  u8_t created;
  u8_t free;
};

static struct sem_slot sem_pool[LWIP_SEM_COUNT];
static int sem_pool_ready;

#ifdef LWIP_SEM_IDS
static const ID sem_id_table[LWIP_SEM_COUNT] = LWIP_SEM_IDS;
#endif

static void sem_pool_init(void)
{
  int i;

  if (sem_pool_ready) {
    return;
  }

  for (i = 0; i < LWIP_SEM_COUNT; i++) {
#ifdef LWIP_SEM_IDS
    sem_pool[i].id = sem_id_table[i];
#else
    sem_pool[i].id = (ID)(LWIP_SEM_ID_BASE + i);
#endif
    sem_pool[i].created = 0;
    sem_pool[i].free = 1;
  }
  sem_pool_ready = 1;
}

static void sem_drain(ID id)
{
  while (pol_sem(id) == E_OK) {
    /* drain resource count to 0 */
  }
}

static ER sem_slot_ensure_created(struct sem_slot *slot)
{
  T_CSEM csem;

  if (slot->created) {
    return E_OK;
  }

  csem.sematr = TA_TFIFO;
  csem.isemcnt = 0;
  csem.maxsem = TMAX_MAXSEM;
  if (cre_sem(slot->id, &csem) != E_OK) {
    return E_OBJ;
  }
  slot->created = 1;
  return E_OK;
}

static struct sys_sem *sem_alloc_from_pool(u8_t count)
{
  int i;
  struct sys_sem *sem;
  UINT n;

  sem_pool_init();

  for (i = 0; i < LWIP_SEM_COUNT; i++) {
    if (sem_pool[i].free) {
      if (sem_slot_ensure_created(&sem_pool[i]) != E_OK) {
        return NULL;
      }
      sem_drain(sem_pool[i].id);
      for (n = 0; n < count; n++) {
        if (sig_sem(sem_pool[i].id) != E_OK) {
          sem_drain(sem_pool[i].id);
          return NULL;
        }
      }
      sem_pool[i].free = 0;

      sem = (struct sys_sem *)malloc(sizeof(*sem));
      if (sem == NULL) {
        sem_drain(sem_pool[i].id);
        sem_pool[i].free = 1;
        return NULL;
      }
      sem->id = sem_pool[i].id;
      sem->slot = i;
      return sem;
    }
  }
  return NULL;
}

static void sem_release_to_pool(struct sys_sem *sem)
{
  if (sem == NULL) {
    return;
  }
  if (sem->slot >= 0 && sem->slot < LWIP_SEM_COUNT) {
    sem_drain(sem->id);
    sem_pool[sem->slot].free = 1;
  }
  free(sem);
}

/* -------------------------------------------------------------------------- */
/* Time                                                                       */
/* -------------------------------------------------------------------------- */

u32_t sys_now(void)
{
  SYSTIM tim;

  get_tim(&tim);
  return (u32_t)tim.st_low;
}

u32_t sys_jiffies(void)
{
  return sys_now();
}

/* TMO is a short on many ittrium ports — slice long waits */
#ifndef LWIP_TMO_SLICE_MS
#define LWIP_TMO_SLICE_MS  10000
#endif

static ER sem_wait_ms(ID id, u32_t timeout_ms)
{
  u32_t left;
  TMO slice;
  ER er;

  if (timeout_ms == 0) {
    /* lwIP: 0 means wait forever */
    return wai_sem(id);
  }

  left = timeout_ms;
  while (left > 0) {
    if (left > (u32_t)LWIP_TMO_SLICE_MS) {
      slice = (TMO)LWIP_TMO_SLICE_MS;
    } else {
      slice = (TMO)left;
    }
    er = twai_sem(id, slice);
    if (er == E_OK) {
      return E_OK;
    }
    if (er != E_TMOUT) {
      return er;
    }
    left -= (u32_t)slice;
  }
  return E_TMOUT;
}

/* -------------------------------------------------------------------------- */
/* Semaphores                                                                 */
/* -------------------------------------------------------------------------- */

err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
  struct sys_sem *s;

  LWIP_ASSERT("sem != NULL", sem != NULL);

  s = sem_alloc_from_pool(count);
  if (s == NULL) {
    return ERR_MEM;
  }
  *sem = s;
  SYS_STATS_INC_USED(sem);
  return ERR_OK;
}

void sys_sem_free(sys_sem_t *sem)
{
  LWIP_ASSERT("sem != NULL", sem != NULL);
  if (*sem != NULL) {
    sem_release_to_pool(*sem);
    *sem = NULL;
    SYS_STATS_DEC(sem.used);
  }
}

void sys_sem_signal(sys_sem_t *sem)
{
  LWIP_ASSERT("sem != NULL", sem != NULL);
  LWIP_ASSERT("*sem != NULL", *sem != NULL);
  (void)sig_sem((*sem)->id);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  u32_t start;
  ER er;

  LWIP_ASSERT("sem != NULL", sem != NULL);
  LWIP_ASSERT("*sem != NULL", *sem != NULL);

  start = sys_now();
  er = sem_wait_ms((*sem)->id, timeout);
  if (er != E_OK) {
    return SYS_ARCH_TIMEOUT;
  }
  return sys_now() - start;
}

/* -------------------------------------------------------------------------- */
/* Mutex (binary semaphore)                                                   */
/* -------------------------------------------------------------------------- */

struct sys_mutex {
  struct sys_sem *sem;
};

err_t sys_mutex_new(sys_mutex_t *mutex)
{
  struct sys_mutex *m;
  struct sys_sem *s;

  LWIP_ASSERT("mutex != NULL", mutex != NULL);

  m = (struct sys_mutex *)malloc(sizeof(*m));
  if (m == NULL) {
    return ERR_MEM;
  }
  s = sem_alloc_from_pool(1);
  if (s == NULL) {
    free(m);
    return ERR_MEM;
  }
  m->sem = s;
  *mutex = m;
  SYS_STATS_INC_USED(mutex);
  return ERR_OK;
}

void sys_mutex_free(sys_mutex_t *mutex)
{
  LWIP_ASSERT("mutex != NULL", mutex != NULL);
  if (*mutex != NULL) {
    sem_release_to_pool((*mutex)->sem);
    free(*mutex);
    *mutex = NULL;
    SYS_STATS_DEC(mutex.used);
  }
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
  LWIP_ASSERT("mutex != NULL", mutex != NULL);
  LWIP_ASSERT("*mutex != NULL", *mutex != NULL);
  (void)wai_sem((*mutex)->sem->id);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
  LWIP_ASSERT("mutex != NULL", mutex != NULL);
  LWIP_ASSERT("*mutex != NULL", *mutex != NULL);
  (void)sig_sem((*mutex)->sem->id);
}

/* -------------------------------------------------------------------------- */
/* Mailbox: ring buffer of void* + wait semaphores                            */
/* -------------------------------------------------------------------------- */

struct sys_mbox {
  void **q;
  int size;
  int head;
  int tail;
  int used;
  struct sys_sem *sem_empty; /* wake fetchers when a message is queued */
  struct sys_sem *sem_full;  /* wake posters when a slot frees */
};

static void mbox_enqueue_unlocked(struct sys_mbox *m, void *msg)
{
  m->q[m->head] = msg;
  m->head++;
  if (m->head >= m->size) {
    m->head = 0;
  }
  m->used++;
}

static void *mbox_dequeue_unlocked(struct sys_mbox *m)
{
  void *msg;

  msg = m->q[m->tail];
  m->q[m->tail] = NULL;
  m->tail++;
  if (m->tail >= m->size) {
    m->tail = 0;
  }
  m->used--;
  return msg;
}

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  struct sys_mbox *m;

  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_ASSERT("size > 0", size > 0);

  if (size > (int)TMAX_MAXSEM) {
    return ERR_MEM;
  }

  m = (struct sys_mbox *)calloc(1, sizeof(*m));
  if (m == NULL) {
    return ERR_MEM;
  }

  m->q = (void **)calloc((size_t)size, sizeof(void *));
  if (m->q == NULL) {
    free(m);
    return ERR_MEM;
  }

  /* Both start at 0; capacity is enforced via `used` under CS. */
  m->sem_empty = sem_alloc_from_pool(0);
  m->sem_full = sem_alloc_from_pool(0);
  if (m->sem_empty == NULL || m->sem_full == NULL) {
    if (m->sem_empty) {
      sem_release_to_pool(m->sem_empty);
    }
    if (m->sem_full) {
      sem_release_to_pool(m->sem_full);
    }
    free(m->q);
    free(m);
    return ERR_MEM;
  }

  m->size = size;
  m->head = 0;
  m->tail = 0;
  m->used = 0;
  *mbox = m;
  SYS_STATS_INC_USED(mbox);
  return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
  struct sys_mbox *m;

  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  m = *mbox;
  if (m == NULL) {
    return;
  }

  LWIP_ASSERT("mbox empty", m->used == 0);

  sem_release_to_pool(m->sem_empty);
  sem_release_to_pool(m->sem_full);
  free(m->q);
  free(m);
  *mbox = NULL;
  SYS_STATS_DEC(mbox.used);
}

static err_t mbox_try_enqueue(struct sys_mbox *m, void *msg, int from_isr)
{
  lock_state_t ls;

  begin_critical_section(ls);
  if (m->used >= m->size) {
    end_critical_section(ls);
    return ERR_MEM;
  }
  mbox_enqueue_unlocked(m, msg);
  end_critical_section(ls);

  if (from_isr) {
    (void)isig_sem(m->sem_empty->id);
  } else {
    (void)sig_sem(m->sem_empty->id);
  }
  return ERR_OK;
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  struct sys_mbox *m;

  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  m = *mbox;
  LWIP_ASSERT("m != NULL", m != NULL);

  for (;;) {
    if (mbox_try_enqueue(m, msg, 0) == ERR_OK) {
      return;
    }
    (void)wai_sem(m->sem_full->id);
  }
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_ASSERT("*mbox != NULL", *mbox != NULL);
  return mbox_try_enqueue(*mbox, msg, 0);
}

err_t sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg)
{
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_ASSERT("*mbox != NULL", *mbox != NULL);
  return mbox_try_enqueue(*mbox, msg, 1);
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  struct sys_mbox *m;
  u32_t start;
  ER er;
  void *mmsg;
  int was_full;
  lock_state_t ls;

  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  m = *mbox;
  LWIP_ASSERT("m != NULL", m != NULL);

  start = sys_now();
  for (;;) {
    er = sem_wait_ms(m->sem_empty->id, timeout);
    if (er != E_OK) {
      return SYS_ARCH_TIMEOUT;
    }

    begin_critical_section(ls);
    if (m->used == 0) {
      end_critical_section(ls);
      if (timeout != 0) {
        u32_t waited = sys_now() - start;
        if (waited >= timeout)
          return SYS_ARCH_TIMEOUT;
        timeout -= waited;
        start = sys_now();
      }
      continue;
    }
    was_full = (m->used >= m->size);
    mmsg = mbox_dequeue_unlocked(m);
    end_critical_section(ls);

    if (was_full)
      (void)sig_sem(m->sem_full->id);

    if (msg != NULL) {
      *msg = mmsg;
    }
    return sys_now() - start;
  }
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  struct sys_mbox *m;
  void *mmsg;
  int was_full;
  lock_state_t ls;

  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  m = *mbox;
  LWIP_ASSERT("m != NULL", m != NULL);

  if (pol_sem(m->sem_empty->id) != E_OK) {
    return SYS_MBOX_EMPTY;
  }

  begin_critical_section(ls);
  if (m->used == 0) {
    end_critical_section(ls);
    return SYS_MBOX_EMPTY;
  }
  was_full = (m->used >= m->size);
  mmsg = mbox_dequeue_unlocked(m);
  end_critical_section(ls);

  if (was_full)
    (void)sig_sem(m->sem_full->id);

  if (msg != NULL) {
    *msg = mmsg;
  }
  return 0;
}

/* -------------------------------------------------------------------------- */
/* Threads                                                                    */
/* -------------------------------------------------------------------------- */

struct lwip_thread_slot {
  ID id;
  u8_t used;
  lwip_thread_fn fn;
  void *arg;
  UB stack[LWIP_THREAD_STACK_SIZE]
#if defined(__GNUC__)
    __attribute__((aligned(16)))
#endif
    ;
#ifdef USE_SEPARATE_STACK
  UB sstack[LWIP_THREAD_STACK_SIZE]
#if defined(__GNUC__)
    __attribute__((aligned(16)))
#endif
    ;
#endif
};

#ifdef LWIP_TASK_IDS
static const ID task_id_table[LWIP_THREAD_COUNT] = LWIP_TASK_IDS;
#endif

static struct lwip_thread_slot thread_pool[LWIP_THREAD_COUNT];
static int thread_pool_inited;

static void thread_pool_init_ids(void)
{
  int i;

  if (thread_pool_inited) {
    return;
  }
  for (i = 0; i < LWIP_THREAD_COUNT; i++) {
#ifdef LWIP_TASK_IDS
    thread_pool[i].id = task_id_table[i];
#else
    thread_pool[i].id = (ID)(LWIP_TASK_ID_BASE + i);
#endif
    thread_pool[i].used = 0;
    thread_pool[i].fn = NULL;
    thread_pool[i].arg = NULL;
  }
  thread_pool_inited = 1;
}

static void lwip_thread_entry(VP exinf)
{
  struct lwip_thread_slot *slot = (struct lwip_thread_slot *)exinf;
  lwip_thread_fn fn;
  void *arg;

  fn = slot->fn;
  arg = slot->arg;
  if (fn != NULL) {
    fn(arg);
  }
  /* lwIP threads normally never return */
  for (;;) {
    dly_tsk(1000);
  }
}

sys_thread_t
sys_thread_new(const char *name, lwip_thread_fn thread, void *arg,
               int stacksize, int prio)
{
  int i;
  struct lwip_thread_slot *slot;
  T_CTSK ctsk;
  ER er;

  LWIP_UNUSED_ARG(stacksize);

  thread_pool_init_ids();

  slot = NULL;
  BEGIN_CRITICAL_SECTION;
  for (i = 0; i < LWIP_THREAD_COUNT; i++) {
    if (!thread_pool[i].used) {
      thread_pool[i].used = 1;
      slot = &thread_pool[i];
      break;
    }
  }
  END_CRITICAL_SECTION;

  if (slot == NULL) {
    LWIP_ASSERT("lwIP thread pool exhausted", 0);
    return (sys_thread_t)0;
  }

  slot->fn = thread;
  slot->arg = arg;

  memset(&ctsk, 0, sizeof(ctsk));
  ctsk.tskatr = TA_HLNG;
  ctsk.exinf = (VP_INT)slot;
  ctsk.task = (FP)lwip_thread_entry;
  ctsk.itskpri = (PRI)((prio > 0) ? prio : TMIN_TPRI);
  ctsk.stksz = (SIZE)LWIP_THREAD_STACK_SIZE;
  ctsk.stk = (VP)slot->stack;
#ifdef USE_SEPARATE_STACK
  ctsk.sstk = (VP)slot->sstack;
#endif
  if (name && name[0]) {
    ctsk.tskatr |= TA_NAME;
    ctsk.name = name;
  }

  er = cre_tsk(slot->id, &ctsk);
  if (er != E_OK) {
    slot->used = 0;
    LWIP_ASSERT("cre_tsk failed", 0);
    return (sys_thread_t)0;
  }

  er = act_tsk(slot->id);
  if (er != E_OK) {
    slot->used = 0;
    LWIP_ASSERT("act_tsk failed", 0);
    return (sys_thread_t)0;
  }

  return slot->id;
}

/* -------------------------------------------------------------------------- */
/* sys_init                                                                   */
/* -------------------------------------------------------------------------- */

void sys_init(void)
{
  sem_pool_init();
  thread_pool_init_ids();
}

/* Optional: sleep helper used by some lwIP versions */
/* sys_msleep(): use lwIP default in core/sys.c (semaphore-based) */


