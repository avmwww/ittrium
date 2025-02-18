#ifndef _CPU_SUPPORT_H_
#define _CPU_SUPPORT_H_

#include <intrinsics.h>

/******************************************************************************
 Data Types
 ******************************************************************************/
typedef char                B; // Signed 8-bit integer
typedef short               H; // Signed 16-bit integer
typedef long                W; // Signed 32-bit integer
typedef long                D; // Signed 64-bit integer
typedef unsigned char      UB; // Unsigned 8-bit integer
typedef unsigned short     UH; // Unsigned 16-bit integer
typedef unsigned long      UW; // Unsigned 32-bit integer
typedef unsigned           UD; // Unsigned 64-bit integer
typedef char               VB; // 8-bit value with unknown data type
typedef short              VH; // 16-bit value with unknown data type
typedef long               VW; // 32-bit value with unknown data type
typedef long               VD; // 64-bit value with unknown data type
typedef void              *VP; // Pointer to an unknown data type
typedef void      (*FP)(void); // Processing unit start address (pointer to a function)
typedef void     (*FP_VP)(VP); // Processing unit start address (pointer to a function)
typedef int               INT; // Signed integer for the processor
typedef unsigned int     UINT; // Unsigned integer for the processor
typedef unsigned short   BOOL; // Boolean value (TRUE or FALSE)
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
//typedef UD             SYSTIM; // System time (unsigned integer, unit of time is implementation-defined)
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


#if 1
// For IAR compiler inline used as pragma directive
# define INLINE_PRAGMA
# define INLINE inline
// IAR Compiler
extern void dispatch(void);
//# define dispatch()               dispath()
//# define dispatch()             __software_interrupt(0);

# define cpu_lock()             __disable_interrupt()
# define cpu_unlock()           __enable_interrupt()

# define cpu_nop()              __no_operation()
# define cpu_wait()             __wait_for_interrupt()

# define cpu_store_lock_state()   __istate_t cpu_lock_state = __get_interrupt_state()
# define cpu_restore_lock_state()  __set_interrupt_state(cpu_lock_state)

#else
# define INLINE inline
// Don't switch from User stack(don't care flag U)
#define dispatch()    asm("   INT #0")
// Switch from User stack(clear flag U)
//# define dispatch()    asm("   BRK")
# define cpu_store_lock_state()   asm("    PUSHC FLG")
# define cpu_restore_lock_state()   asm("    POPC FLG")
# define cpu_lock()      asm("     FCLR I")
# define cpu_unlock()    asm("     FSET I")
# define cpu_nop()       asm("     NOP")
# define cpu_wait()      asm("     WAIT")
#endif

//------------------------------------------------------------------------------


#define BEGIN_CRITICAL_SECTION	{ cpu_store_lock_state(); cpu_lock()
#define END_CRITICAL_SECTION	cpu_restore_lock_state(); }

void _install_handler(FP handler, INHNO vec_no);
#define low_level_init()

#endif // _CPU_SUPPORT_H_

