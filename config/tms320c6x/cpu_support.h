#ifndef _CPU_SUPPORT_H_
#define _CPU_SUPPORT_H_

#include "tms320c67x.h"
/******************************************************************************
 Data Types
 ******************************************************************************/
typedef char                B; // Signed 8-bit integer
typedef short               H; // Signed 16-bit integer
typedef int                 W; // Signed 32-bit integer
// 40 bits
typedef long                D; // Signed 64-bit integer
typedef unsigned char      UB; // Unsigned 8-bit integer
typedef unsigned short     UH; // Unsigned 16-bit integer
typedef unsigned int       UW; // Unsigned 32-bit integer
// 40 bits
typedef unsigned long      UD; // Unsigned 64-bit integer
typedef char               VB; // 8-bit value with unknown data type
typedef short              VH; // 16-bit value with unknown data type
typedef int                VW; // 32-bit value with unknown data type
// 40 bits
typedef long               VD; // 64-bit value with unknown data type
typedef void              *VP; // Pointer to an unknown data type
typedef void      (*FP)(void); // Processing unit start address (pointer to a function)
typedef void      (*FP_VP)(VP);// Processing unit start address (pointer to a function)
typedef int               INT; // Signed integer for the processor
typedef unsigned int     UINT; // Unsigned integer for the processor
typedef unsigned int     BOOL; // Boolean value (TRUE or FALSE)
#define FALSE            (0)
#define TRUE             (1)
typedef int                FN; // Function code (signed integer)
typedef int                ER; // Error code (signed integer)
typedef int                ID; // Object ID number (signed integer)
typedef unsigned int      ATR; // Object attribute (unsigned integer)
typedef unsigned int     STAT; // Object state (unsigned integer)
typedef unsigned int     MODE; // Service call operational mode (unsigned integer)
typedef int               PRI; // Priority (signed integer)
typedef unsigned int     SIZE; // Memory area size (unsigned integer)
typedef int               TMO; // Timeout (signed integer, unit of time is implementation-defined)
typedef unsigned int   RELTIM; // Relative time (unsigned integer, unit of time is implementation-defined)
//typedef UD             SYSTIM; // System time (unsigned integer, unit of time is implementation-defined)
typedef struct{
  UW st_low;
  UW st_high;
}SYSTIM;
typedef int           *VP_INT; // Pointer to an unknown data type, or a signed integer for the processor
typedef int           ER_BOOL; // Error code or a boolean value (signed integer)
typedef int             ER_ID; // Error code or an object ID number (signed integers and negative ID numbers cannot be represented)
typedef UINT          ER_UINT; // Error code or an unsigned integer (the number of available bits for an unsigned integer is one bit shorter than UINT)
typedef UINT           TEXPTN; // Bit pattern for the task exception code (unsigned integer)
typedef UINT           FLGPTN; // Bit pattern of the eventflag (unsigned integer)
typedef UH              INHNO; // Interrupt handler number
typedef UH              INTNO; // Interrupt number
//******************************************************************************
#define INLINE         inline
// int is 32 bits long, bitmap = 32 bits, shift = 5
#define BITMAPSHIFT          5

typedef struct {
  VP   sp;   /* SP (Stack Pointer) */
} CTXB;

//#define dispatch()    asm("   INTR #31")
void dispatch(void);
//------------------------------------------------------------------------------
#define cpu_is_locked()  (!(CSR & 1))
#define cpu_lock()      CSR &= ~1;
#define cpu_unlock()    CSR |= 1;
#define cpu_nop()       asm("     NOP")

#define BEGIN_CRITICAL_SECTION	{ BOOL cpu_locked = cpu_is_locked(); cpu_lock();
#define END_CRITICAL_SECTION	if(!cpu_locked) cpu_unlock(); }

void install_handler(FP handler, INHNO vec_no);

void low_level_init(void);

#endif // _CPU_SUPPORT_H_

