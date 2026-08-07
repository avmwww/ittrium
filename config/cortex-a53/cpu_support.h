#ifndef _CPU_SUPPORT_H_
#define _CPU_SUPPORT_H_

#include <stdint.h>
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
	VP sp;
	VP pc;
} CTXB;

#define INLINE static inline

void dispatch(void);

static inline void cpu_lock(void)
{
	__asm__ volatile("msr daifset, #2" ::: "memory");
}

static inline void cpu_unlock(void)
{
	__asm__ volatile("msr daifclr, #2" ::: "memory");
}

static inline void cpu_nop(void)
{
	__asm__ volatile("nop");
}

static inline void cpu_wait(void)
{
	__asm__ volatile("wfi");
}

static inline uint64_t cpu_get_daif(void)
{
	uint64_t v;
	__asm__ volatile("mrs %0, daif" : "=r"(v));
	return v;
}

static inline void cpu_set_daif(uint64_t v)
{
	__asm__ volatile("msr daif, %0" :: "r"(v) : "memory");
}

#define cpu_store_lock_state()     uint64_t __daif_save = cpu_get_daif()
#define cpu_restore_lock_state()   cpu_set_daif(__daif_save)

#define BEGIN_CRITICAL_SECTION	{ cpu_store_lock_state(); cpu_lock()
#define END_CRITICAL_SECTION	cpu_restore_lock_state(); }

typedef uint64_t lock_state_t;
#define begin_critical_section(_ctx)    do { (_ctx) = cpu_get_daif(); cpu_lock(); } while (0)
#define end_critical_section(_ctx)      do { cpu_set_daif(_ctx); } while (0)

void install_handler(FP handler, INHNO vec_no, UB prio);
void _int_init(void);
void low_level_init(void);

void gic_init(void);
void gic_enable_irq(unsigned int irq, UB prio);
uint32_t gic_acknowledge(void);
void gic_eoi(uint32_t iar);
void timer_hw_program(void);

#define CLEAR_TICKER_INT()      timer_hw_program()
#define DISABLE_TICKER_INT()    do { \
	uint64_t _ctl; \
	__asm__ volatile("mrs %0, cntp_ctl_el0" : "=r"(_ctl)); \
	_ctl &= ~1ULL; \
	__asm__ volatile("msr cntp_ctl_el0, %0" :: "r"(_ctl)); \
} while (0)
#define ENABLE_TICKER_INT()     do { \
	timer_hw_program(); \
	uint64_t _ctl = 1; \
	__asm__ volatile("msr cntp_ctl_el0, %0" :: "r"(_ctl)); \
	gic_enable_irq(TIMER_IRQ, TICKER_PRIO); \
} while (0)

#define TICKER_PRIO             0x80

enum {
	DISPATCH_VEC_NO = 0,
	TICKER_VEC_NO,
	UART_VEC_NO,
	VEC_NO_LAST,
};

void vectors_start(void);

#endif /* _CPU_SUPPORT_H_ */
