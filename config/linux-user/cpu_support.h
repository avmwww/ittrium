#ifndef _CPU_SUPPORT_H_
#define _CPU_SUPPORT_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdint.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>
#include "target.h"

/******************************************************************************
 Data Types
 ******************************************************************************/
typedef int8_t              B;
typedef int16_t             H;
typedef int32_t             W;
typedef int64_t             D;
typedef uint8_t            UB;
typedef uint16_t           UH;
typedef uint32_t           UW;
typedef uint64_t           UD;
typedef char               VB;
typedef short              VH;
typedef long               VW;
typedef long long          VD;
typedef void              *VP;
typedef void      (*FP)(void);
typedef void     (*FP_VP)(VP);
typedef int               INT;
typedef unsigned int     UINT;
typedef unsigned int     BOOL;
#define FALSE            (0)
#define TRUE             (1)

typedef short              FN;
typedef short              ER;
typedef short              ID;
typedef unsigned short    ATR;
typedef unsigned short   STAT;
typedef unsigned short   MODE;
typedef short             PRI;
typedef unsigned short   SIZE;
typedef H                 TMO;
typedef UINT           RELTIM;
typedef struct {
	UW st_low;
	UW st_high;
} SYSTIM;
typedef int           *VP_INT;
typedef short         ER_BOOL;
typedef short           ER_ID;
typedef UINT          ER_UINT;
typedef UINT           TEXPTN;
typedef UINT           FLGPTN;
typedef UH              INHNO;
typedef UH              INTNO;

#define BITMAPSHIFT      4

typedef struct {
	ucontext_t uc;
	FP_VP entry;
	VP arg;
} CTXB;

void dispatch(void);

extern sigset_t linux_user_irq_sigset;

static inline void cpu_lock(void)
{
	sigprocmask(SIG_BLOCK, &linux_user_irq_sigset, NULL);
}

static inline void cpu_unlock(void)
{
	sigprocmask(SIG_UNBLOCK, &linux_user_irq_sigset, NULL);
}

static inline void cpu_nop(void)
{
}

static inline void cpu_wait(void)
{
	pause();
}

#define cpu_store_lock_state()     sigset_t __sig_save; sigprocmask(0, NULL, &__sig_save)
#define cpu_restore_lock_state()   sigprocmask(SIG_SETMASK, &__sig_save, NULL)

#define BEGIN_CRITICAL_SECTION	{ cpu_store_lock_state(); cpu_lock()
#define END_CRITICAL_SECTION	cpu_restore_lock_state(); }

typedef sigset_t lock_state_t;
#define begin_critical_section(_ctx)    do { sigprocmask(0, NULL, &(_ctx)); cpu_lock(); } while (0)
#define end_critical_section(_ctx)      do { sigprocmask(SIG_SETMASK, &(_ctx), NULL); } while (0)

void install_handler(FP handler, INHNO vec_no, UB prio);

extern volatile UW irq_count[32];
const char *irq_vec_name(unsigned vec);
void _int_init(void);
void low_level_init(void);

#define CLEAR_TICKER_INT()      do { } while (0)
#define DISABLE_TICKER_INT()    linux_user_timer_stop()
#define ENABLE_TICKER_INT()     linux_user_timer_start()

void linux_user_timer_start(void);
void linux_user_timer_stop(void);

#define TICKER_PRIO             0x80

enum {
	DISPATCH_VEC_NO = 0,
	TICKER_VEC_NO,
	UART_VEC_NO,
	NETDEV_VEC_NO,
	VEC_NO_LAST,
};

#endif /* _CPU_SUPPORT_H_ */
