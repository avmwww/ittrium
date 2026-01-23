#ifndef _CPU_SUPPORT_H_
#define _CPU_SUPPORT_H_

#include <stdint.h>
#include "target.h"
#include <core_cm3.h>

/******************************************************************************
 Data Types
 ******************************************************************************/
typedef int8_t              B; // Signed 8-bit integer
typedef int16_t             H; // Signed 16-bit integer
typedef int32_t             W; // Signed 32-bit integer
typedef int64_t             D; // Signed 64-bit integer
typedef uint8_t            UB; // Unsigned 8-bit integer
typedef uint16_t           UH; // Unsigned 16-bit integer
typedef uint32_t           UW; // Unsigned 32-bit integer
typedef uint64_t           UD; // Unsigned 64-bit integer
typedef char               VB; // 8-bit value with unknown data type
typedef short              VH; // 16-bit value with unknown data type
typedef long               VW; // 32-bit value with unknown data type
typedef long               VD; // 64-bit value with unknown data type
typedef void              *VP; // Pointer to an unknown data type
typedef void      (*FP)(void); // Processing unit start address (pointer to a function)
typedef void     (*FP_VP)(VP); // Processing unit start address (pointer to a function)
typedef int               INT; // Signed integer for the processor
typedef unsigned int     UINT; // Unsigned integer for the processor
typedef unsigned int     BOOL; // Boolean value (TRUE or FALSE)
#define FALSE            (0)
#define TRUE             (1)

typedef short              FN; // Function code (signed integer)
typedef short              ER; // Error code (signed integer)
typedef short              ID; // Object ID number (signed integer)
typedef unsigned short    ATR; // Object attribute (unsigned integer)
typedef unsigned short   STAT; // Object state (unsigned integer)
typedef unsigned short   MODE; // Service call operational mode (unsigned integer)
typedef short             PRI; // Priority (signed integer)
typedef unsigned short   SIZE; // Memory area size (unsigned integer)
typedef H                 TMO; // Timeout (signed integer, unit of time is implementation-defined)
typedef UINT           RELTIM; // Relative time (unsigned integer, unit of time is implementation-defined)
typedef struct{
    UW st_low;
    UW st_high;
}SYSTIM;
typedef int           *VP_INT; // Pointer to an unknown data type, or a signed integer for the processor
typedef short         ER_BOOL; // Error code or a boolean value (signed integer)
typedef short           ER_ID; // Error code or an object ID number (signed integers and negative ID numbers cannot be represented)
typedef UINT          ER_UINT; // Error code or an unsigned integer (the number of available bits for an unsigned integer is one bit shorter than UINT)
typedef UINT           TEXPTN; // Bit pattern for the task exception code (unsigned integer)
typedef UINT           FLGPTN; // Bit pattern of the eventflag (unsigned integer)
typedef UH              INHNO; // Interrupt handler number
typedef UH              INTNO; // Interrupt number
//******************************************************************************
#define BITMAPSHIFT      4

typedef struct {
  VP   sp;   /* SP (Stack Pointer) */
  VP   pc;
} CTXB;

#define INLINE static inline

void _dispatch(void);
#if 1
# define dispatch()                 do {SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;} while (0)
//# define dispatch()                 NVIC_SetPendingIRQ(PendSV_IRQn);
#else
# define dispatch()                 do {__enable_irq(); __asm volatile("SVC #0\n");} while (0)
#endif
#define cpu_lock()                 __disable_irq()
#define cpu_unlock()               __enable_irq();
#define cpu_nop()                  __NOP();
#define cpu_wait()                 __WFI();
#define cpu_store_lock_state()     uint32_t primask_bit = __get_PRIMASK()
#define cpu_restore_lock_state()   __set_PRIMASK(primask_bit)

#ifdef IAR
// For IAR compiler inline used as pragma directive
# define INLINE_PRAGMA
#endif

//------------------------------------------------------------------------------

#define BEGIN_CRITICAL_SECTION	{ cpu_store_lock_state(); cpu_lock()
#define END_CRITICAL_SECTION	cpu_restore_lock_state(); }

typedef uint32_t lock_state_t;
#define begin_critical_section(_ctx)    do { _ctx = __get_PRIMASK(); cpu_lock(); } while (0)
#define end_critical_section(_ctx)      do {__set_PRIMASK(ctx);} while (0)

void install_handler(FP handler, INHNO vec_no, UB prio);
void _int_init(void);
void low_level_init(void);

/* Clear the timer interrupt */
#define CLEAR_TICKER_INT()      NVIC_ClearPendingIRQ(SysTick_IRQn)
#define DISABLE_TICKER_INT()    NVIC_DisableIRQ(SysTick_IRQn)
#define ENABLE_TICKER_INT()     NVIC_EnableIRQ(SysTick_IRQn)

#define TICKER_PRIO             1
// Internal ittrium vector numbers
enum {
    DISPATCH_VEC_NO = 0,
    TICKER_VEC_NO,
    VEC_NO_LAST,
};

/**
 *
 */
void _interrupt_handler(INHNO vector) __attribute__((noreturn));

#define interrupt_handler(_vec)                     \
{                                                   \
    __asm__ volatile (                              \
            "mov r0, %1\n"                          \
            "orr r1, %0, #1\n"                      \
            "bx r1\n"                               \
            : /* No outputs */                      \
            : "r" (_interrupt_handler), "i" (_vec)  \
            : /* No Clobber */                      \
            );                                      \
}


#endif // _CPU_SUPPORT_H_

