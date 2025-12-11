/**
 * @brief Generic cortex-m3 port
 */
#include "ittrium.h"
#include "../kernel/task.h"
#include "../cpu_generic.c"

uint8_t interrupt_stack[INTERRUPT_STACK_SIZE];
/**
 *
 */
struct ivt_t {
  FP  func;
  UW  prio;
} int_vector_table[32];

static inline void interrupt_handler_service(void)
{
    __asm__ volatile (
        "orr r0, %0, #1\n" // Thumb
        "bx r0\n"
        : // No outputs
        : "r" (_dispatch)
        : // No Clobber
    );
}

static void exit_task(void)
{
    runtsk = (UW)0;
    for (;;);
}

void task_idle_c(void *arg)
{
    for (;;) {
        __WFI();
    }
}

/**
 *
 */
void make_task_context(TCB *tcb)
{
  // Task stack pointer initialy pointed to first byte after task stack
  UW *pstk;

  // Align stack pointer
  pstk = (VP)((UW)tcb->stk & ~7UL);
  // EPSR: xPSR, thumb
  *--pstk = 0x01000000;
  // PC, Mask off lower bit in case task is thumb mode
  *--pstk = (UW)tcb->task & ~1;
  // LR
  *--pstk = (UW)exit_task;
  // R12
  *--pstk = (UW)0x12121212L;
  // R3
  *--pstk = (UW)0x03030303L;
  // R2
  *--pstk = (UW)0x02020202L;
  // R1
  *--pstk = (UW)0x01010101L;
  // R0, function argument
  *--pstk = (UW)tcb->exinf;
  // R11
  *--pstk = (UW)0x11111111L;
  // R10
  *--pstk = (UW)0x10101010L;
  // R9
  *--pstk = (UW)0x09090909L;
  // R8
  *--pstk = (UW)0x08080808L;
  // R7
  *--pstk = (UW)0x07070707L;
  // R6
  *--pstk = (UW)0x06060606L;
  // R5
  *--pstk = (UW)0x05050505L;
  // R4
  *--pstk = (UW)0x04040404L;

  tcb->tskctxb.sp = pstk;
}

/**
 *
 */
void interrupt_handler(INHNO vector)
#if 0
{
    // TODO: interrupt stack use on irq
  if (int_vector_table[vector].func) {
    int_vector_table[vector].func();
  }
}
#else
;
#endif

/**
 *
 */
void install_handler(FP handler, INHNO vec_no, UB prio)
{
  if (vec_no < 32) {
    BEGIN_CRITICAL_SECTION;
    int_vector_table[vec_no].func = handler;
    int_vector_table[vec_no].prio = prio;
    END_CRITICAL_SECTION;
  }
}

/**
 *
 */
void _int_init(void)
{
  int i;

  int_nesting = 0;
  for (i = 0;  i < 32; i++) {
    int_vector_table[i].func = (FP)0;
    int_vector_table[i].prio = 0;
  }
}

/**
 *
 */
void start_hw_timer(void)
{
    timer_nesting = 0;
    INIT_SYSTEM_TICKS();

    install_handler(timer_handler, TICKER_VEC_NO, TICKER_PRIO);

    ENABLE_TICKER_INT();
}

/**
 *
 */
void terminate_hw_timer(void)
{
  DISABLE_TICKER_INT();
  int_vector_table[TICKER_VEC_NO].func = (FP)0;
  int_vector_table[TICKER_VEC_NO].prio = 0;
}

/**
 *
 */
__weak void _low_level_init(void)
{
}

void low_level_init(void)
{
    _int_init();
    _low_level_init();
}

/*
 * Cortex-M3 Processor Interruption and Exception Handlers
 */

static inline void interrupt_handler_goto(int vector)
{
    __asm__ volatile (
        "mov r1, %0\n"
        "mov r0, %1\n"
        "orr r1, #1\n"
        "bx r1\n"
        : // No outputs
        : "r" (interrupt_handler), "r" (vector)
        : // No Clobber
    );
}

static inline void system_fault_reset(void)
{
    NVIC_SystemReset();
    while (1) {}
}

void NMI_Handler(void)
{
    while (1) {}
}

void HardFault_Handler(void)
{
    system_fault_reset();
}

void MemManage_Handler(void)
{
    system_fault_reset();
}

void BusFault_Handler(void)
{
    system_fault_reset();
}

void UsageFault_Handler(void)
{
    system_fault_reset();
}

/*
 * System service call via SWI instruction
 */
__attribute__((naked)) void SVC_Handler(void)
{
    interrupt_handler_service();
}

void DebugMon_Handler(void)
{
}

/*
 * Pendable request for system service
 */
void PendSV_Handler(void)
{
    interrupt_handler_service();
}

__attribute__((naked)) void SysTick_Handler(void)
{
    interrupt_handler_goto(TICKER_VEC_NO);
}

