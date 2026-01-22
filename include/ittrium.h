#ifndef _ITRON_H_
#define _ITRON_H_

#include <stdarg.h>
#include "cpu_support.h"

#define TSK_SELF        0
/******************************************************************************
 Task states
 ******************************************************************************/
#define TTS_NOEXS       0
#define TTS_RUN         0x01 // RUNNING state
#define TTS_RDY         0x02 // READY state
#define TTS_WAI         0x04 // WAITING state
#define TTS_SUS         0x08 // SUSPENDED state
#define TTS_WAS         0x0c // WAITING-SUSPENDED state
#define TTS_DMT         0x10 // DORMANT state
/******************************************************************************
 Atributes
 ******************************************************************************/
#define TA_NULL         0
#define TA_HLNG         0x00 // Start a processing unit through a high-level language interface
#define TA_ASM          0x01 // Start a processing unit through an assembly language interface
#define TA_FPU          0x10 // Floating point unit used by task
#define TA_TFIFO        0x00 // Wait queue will be in FIFO order
#define TA_TPRI         0x01 // Wait queue will be in task priority order
#define TA_MFIFO        0x02
#define TA_MPRI         0x03
#define TA_WSGL         0x00 // Only a single task is allowed to be in the waiting state
#define TA_WMUL         0x02 // Multiple tasks are allowed to be in the waiting state
#define TA_CLR          0x04 // Entire bit pattern will be cleared when a task is released from the waiting state
/******************************************************************************
 ******************************************************************************/
// The task's initial priority
#define TPRI_INI        (0)
// The base priority of the invoking task becomes the target priority
#define TPRI_SELF       (0)
/******************************************************************************
 Timeout
 ******************************************************************************/
#define TMO_POL         ( 0) // Polling
#define TMO_FEVR        (-1) // Waiting forever
#define TMO_NBLK        (-2) // Non-blocking
/******************************************************************************
 If the task is not in the WAITING state, what wait
 ******************************************************************************/
#define TTW_SLP         0x0001 // Sleeping state
#define TTW_DLY         0x0002 // Delayed state
#define TTW_SEM         0x0004 // Waiting state for a semaphore resource
#define TTW_FLG         0x0008 // Waiting state for an eventflag
#define TTW_SDTQ        0x0010 // Sending waiting state for a data queue
#define TTW_RDTQ        0x0020 // Receiving waiting state for a data queue
#define TTW_MBX         0x0040 // Receiving waiting state for a mailbox
#define TTW_MTX         0x0080 // Waiting state for a mutex
#define TTW_SMBF        0x0100 // Sending waiting state for a message buffer
#define TTW_RMBF        0x0200 // Receiving waiting state for a message buffer
#define TTW_CAL         0x0400 // Calling waiting state for a rendezvous
#define TTW_ACP         0x0800 // Accepting waiting state for a rendezvous
#define TTW_RDV         0x1000 // Terminating waiting state for a rendezvous
#define TTW_MPF         0x2000 // Waiting state for a fixed-sized memory block
#define TTW_MPL         0x4000 // Waiting state for a variable-sized memory block
//
#define TWF_ANDW        0x00
#define TWF_ORW         0x01
/******************************************************************************
 Error Code List
 ******************************************************************************/
#define E_OK            (  0) // Completes execution normally
#define E_ERR           ( -1)
#define E_SYS           ( -5) // System error
#define E_NOSPT         ( -9) // Unsupported function
#define E_RSFN          (-10) // Reserved function code
#define E_RSATR         (-11) // Reserved attribute
#define E_PAR           (-17) // Parameter error
#define E_ID            (-18) // Invalid ID number
#define E_CTX           (-25) // Context error
#define E_MACV          (-26) // Memory access violation
#define E_OACV          (-27) // Object access violation
#define E_ILUSE         (-28) // Illegal service call use
#define E_NOMEM         (-33) // Insufficient memory
#define E_NOID          (-34) // No ID number available
#define E_OBJ           (-41) // Object state error
#define E_NOEXS         (-42) // Non-existent object
#define E_QOVR          (-43) // Queue overflow
#define E_RLWAI         (-49) // Forced release from waiting
#define E_TMOUT         (-50) // Polling failure or timeout
#define E_DLT           (-51) // Waiting object deleted
#define E_CLS           (-52) // Waiting object state changed
#define E_WBLK          (-57) // Non-blocking call accepted
#define E_BOVR          (-58) // Buffer overflow

typedef struct t_msg {
  VP   msgque[1];
} T_MSG; // Message header for a mailbox

typedef struct t_msg_pri {
  T_MSG msgque ; /* Message header */
  PRI   msgpri ; /* Message priority */
} T_MSG_PRI ;// Message header with a message priority for a mailbox

typedef UINT           RDVPTN; // Bit pattern of the rendezvous condition (unsigned integer)
typedef UINT            RDVNO; // Rendezvous number
typedef UD             OVRTIM; // Processor time (unsigned integer, unit of time is implementation-defined)
//typedef UINT            INHNO; // Interrupt handler number
//typedef UINT            INTNO; // Interrupt number
//IXXXX Interrupt mask
typedef UINT            EXCNO; // CPU exception handler number
//==============================================================================
typedef struct t_ctsk {
  ATR    tskatr; /* Task attribute */
  VP_INT  exinf; /* Task extended information */
  FP       task; /* Task start address */
  PRI   itskpri; /* Task initial priority */
  SIZE    stksz; /* Task stack size (in bytes) */
  VP        stk; /* Base address of task stack space */
  /* Other implementation specific fields may be added. */
  VP       sstk; /* Base address of task system stack space */
} T_CTSK ;
typedef struct t_rtsk {
  STAT  tskstat; /* Task state */
  PRI    tskpri; /* Task current priority */
  PRI   tskbpri; /* Task base priority */
  STAT  tskwait; /* Reason for waiting */
  ID     wobjid; /* Object ID number for which the task is waiting */
  TMO   lefttmo; /* Remaining time until timeout */
  UINT   actcnt; /* Activation request count */
  UINT   wupcnt; /* Wakeup request count */
  UINT   suscnt; /* Suspension count */
  /* Other implementation specific fields may be added. */
} T_RTSK ;
typedef struct t_rtst {
  STAT  tskstat; /* Task state */
  STAT  tskwait; /* Reason for waiting */
  /* Other implementation specific fields may be added. */
} T_RTST ;
/*******************************************************************************
 Eventflag
 *******************************************************************************/
typedef struct t_cflg {
  ATR flgatr ; /* Eventflag attribute */
  FLGPTN iflgptn ; /* Initial value of the eventflag bit pattern */
  /* Other implementation specific fields may be added. */
} T_CFLG ;
typedef struct t_rflg {
  ID wtskid ; /* ID number of the task at the head of the eventflag's wait queue */
  FLGPTN flgptn ; /* Current eventflag bit pattern */
  /* Other implementation specific fields may be added. */
} T_RFLG ;
/*******************************************************************************
 Mailbox
 *******************************************************************************/
typedef struct t_cmbx {
  ATR mbxatr ;  /* Mailbox attribute */
  PRI maxmpri ; /* Maximum message priority */
  VP mprihd ;   /* Start address of the area for message queue headers for each message priority */
  /* Other implementation specific fields may be added. */
} T_CMBX ;
typedef struct t_rmbx {
  ID wtskid ;      /* ID number of the task at the head of mailbox's wait queue */
  T_MSG * pk_msg ; /* Start address of the message packet at the head of the message queue */
  /* Other implementation specific fields may be added. */
} T_RMBX ;
/*******************************************************************************
 Semaphore
 *******************************************************************************/
typedef struct t_csem {
  ATR sematr ;   /* Semaphore attribute */
  UINT isemcnt ; /* Initial semaphore resource count */
  UINT maxsem ;  /* Maximum semaphore resource count */
  /* Other implementation specific fields may be added. */
} T_CSEM ;
typedef struct t_rsem {
   ID wtskid ;   /* ID number of the task at the head of the semaphore's wait queue */
   UINT semcnt ; /* Current semaphore resource count */
   /* Other implementation specific fields may be added. */
} T_RSEM ;

/*******************************************************************************
 *******************************************************************************/
#define TMIN_TSKID   1
// Minimum task priority
#define TMIN_TPRI    1

#define TMIN_SEMID   1
#define TMIN_FLGID   1
#define TMIN_DTQID   1
#define TMIN_MBXID   1
#define TMIN_MPFID   1
#define TMIN_CYCID   1

#include "kernel_config.h"

#define TNUM_TPRI              (TMAX_TPRI - TMIN_TPRI + 1)

#ifdef TMAX_MBXID
  #define USE_MAILBOX
  #define TNUM_MBXID           (TMAX_MBXID - TMIN_MBXID + 1)
#endif // TMAX_MBXID

#ifdef TMAX_SEMID
  #define USE_SEMAPHORE
  #ifndef TRSV_SEMID
   #define TRSV_SEMID          0 // Reserved for automatic assignment
  #endif
  #define TNUM_SEMID           (TMAX_SEMID - TMIN_SEMID + 1 + TRSV_SEMID)
  #ifndef TMAX_MAXSEM
    #error "TMAX_MAXSEM => Maximum value of the maximum definable semaphore resource count not defined"
  #endif // TMAX_MAXSEM
#endif // TMAX_SEMID

#ifdef TMAX_FLGID
  #define USE_EVENTFLAG
  #define TNUM_FLGID           (TMAX_FLGID - TMIN_FLGID + 1)
#endif // TMAX_FLGID

#ifndef TMAX_TSKID
# error "TMAX_TSKID => Maximum task ID value isn't defined, please define it in a kernel_config.h"
#endif

#define TNUM_TSKID             (TMAX_TSKID - TMIN_TSKID + 1)

#include "service_call.h"

#endif // _ITRON_H_
