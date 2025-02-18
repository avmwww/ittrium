#ifndef _CPU_SUPPORT_H_
#define _CPU_SUPPORT_H_

#include "tms320c55x.h"
/******************************************************************************
 Data Types
 ******************************************************************************/
// On TMS54xx or TMS55x  64bits data types => 40bit
typedef char                B; // Signed 8-bit integer
typedef short               H; // Signed 16-bit integer
typedef long                W; // Signed 32-bit integer
typedef long long           D; // Signed 64-bit integer
typedef unsigned char      UB; // Unsigned 8-bit integer
typedef unsigned short     UH; // Unsigned 16-bit integer
typedef unsigned long      UW; // Unsigned 32-bit integer
typedef unsigned long long UD; // Unsigned 64-bit integer
typedef char               VB; // 8-bit value with unknown data type
typedef short              VH; // 16-bit value with unknown data type
typedef long               VW; // 32-bit value with unknown data type
typedef long long          VD; // 64-bit value with unknown data type
typedef void              *VP; // Pointer to an unknown data type
typedef void      (*FP)(void); // Processing unit start address (pointer to a function)
typedef void      (*FP_VP)(VP); // Processing unit start address (pointer to a function)
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
#define INLINE inline

#define USE_SEPARATE_STACK     1
#define BITMAPSHIFT      4

typedef struct {
  VP   sp;   /* SP (Stack Pointer) */
  VP   ssp;  /* SSP (System Stack Pointer) */
  VP   reta; /* RETA */ 
} CTXB;

#define dispatch()    asm("   INTR #31")

#define ST1_55    (*(volatile unsigned int*)0x0003)
//------------------------------------------------------------------------------
#define cpu_is_locked()  (ST1_55 & 0x0800)
#define cpu_lock()      asm("     BSET ST1_INTM")
#define cpu_unlock()    asm("     BCLR ST1_INTM")
#define cpu_nop()       asm("     NOP")

#define BEGIN_CRITICAL_SECTION	{ BOOL cpu_locked = cpu_is_locked(); cpu_lock();
#define END_CRITICAL_SECTION	if(!cpu_locked) cpu_unlock(); }

void _install_handler(FP handler, INHNO vec_no);
void install_handler(FP handler, INHNO vec_no);

#define low_level_init()

#endif // _CPU_SUPPORT_H_

