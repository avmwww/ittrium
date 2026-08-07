/**
 * lwIP sys_arch types and protection for ittrium (μITRON)
 *
 * kernel_config.h (via ittrium.h) should provide:
 *   LWIP_SEM_ID_BASE, LWIP_SEM_COUNT  — or LWIP_SEM_IDS { ... }
 *   LWIP_TASK_ID_BASE                 — or LWIP_TASK_IDS { ... }
 *   optionally TCPIP_TASK_ID as first thread ID
 *
 * Semaphores are taken from a free list of static IDs.
 * Mailboxes are ring buffers of void* with counting-style wait sems
 * (not ittrium mbx).
 */
#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include "ittrium.h"

#ifdef __cplusplus
extern "C" {
#endif

struct sys_sem;
struct sys_mutex;
struct sys_mbox;

typedef struct sys_sem   *sys_sem_t;
typedef struct sys_mutex *sys_mutex_t;
typedef struct sys_mbox  *sys_mbox_t;
typedef ID                sys_thread_t;

typedef lock_state_t sys_prot_t;

/* SYS_LIGHTWEIGHT_PROT via DAIF save/restore */
#define SYS_ARCH_DECL_PROTECT(lev)  lock_state_t lev
#define SYS_ARCH_PROTECT(lev)       begin_critical_section(lev)
#define SYS_ARCH_UNPROTECT(lev)     end_critical_section(lev)

#define sys_sem_valid(sem)           (((sem) != NULL) && (*(sem) != NULL))
#define sys_sem_set_invalid(sem)     do { if (sem) { *(sem) = NULL; } } while (0)

#define sys_mutex_valid(mutex)       (((mutex) != NULL) && (*(mutex) != NULL))
#define sys_mutex_set_invalid(mutex) do { if (mutex) { *(mutex) = NULL; } } while (0)

#define sys_mbox_valid(mbox)         (((mbox) != NULL) && (*(mbox) != NULL))
#define sys_mbox_set_invalid(mbox)   do { if (mbox) { *(mbox) = NULL; } } while (0)

/* ---- Pool sizing (overridable from kernel_config / lwipopts) ---- */

#ifndef LWIP_SEM_COUNT
#define LWIP_SEM_COUNT  32
#endif

#ifndef LWIP_THREAD_COUNT
#define LWIP_THREAD_COUNT  4
#endif

#ifndef LWIP_THREAD_STACK_SIZE
#define LWIP_THREAD_STACK_SIZE  2048
#endif

/*
 * Semaphore ID pool:
 *   #define LWIP_SEM_IDS { 10, 11, ... }  OR
 *   #define LWIP_SEM_ID_BASE  <first ID>  with LWIP_SEM_COUNT consecutive IDs
 */
#if defined(LWIP_SEM_IDS)
/* IDs provided as brace-initializer list by kernel_config */
#elif defined(LWIP_SEM_ID_BASE)
/* consecutive range starting at LWIP_SEM_ID_BASE */
#else
/* Placeholder defaults — override in kernel_config.h for real boards */
#define LWIP_SEM_ID_BASE  20
#endif

/*
 * Thread ID pool:
 *   #define LWIP_TASK_IDS { TCPIP_TASK_ID, ... }  OR
 *   #define LWIP_TASK_ID_BASE / TCPIP_TASK_ID
 */
#if defined(LWIP_TASK_IDS)
#elif defined(LWIP_TASK_ID_BASE)
#elif defined(TCPIP_TASK_ID)
#define LWIP_TASK_ID_BASE  TCPIP_TASK_ID
#else
#define LWIP_TASK_ID_BASE  10
#endif

#ifdef __cplusplus
}
#endif

#endif /* LWIP_ARCH_SYS_ARCH_H */
