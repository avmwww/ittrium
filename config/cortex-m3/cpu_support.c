/**
 * @brief Generic cortex-m3 port
 */
#include "ittrium.h"
#include "../kernel/task.h"
#include "../cpu_generic.c"

#ifndef EXC_RETURN_THREAD_PSP
# define EXC_RETURN_THREAD_PSP (0xFFFFFFFDUL) /* return to Thread mode, uses PSP after return */
#endif

/**
 *
 */
struct ivt_t {
  FP  func;
  UW  prio;
} int_vector_table[32];

int idle_stack[IDLE_TSK_STACK_SIZE / sizeof(int)];

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
  UW *pstk;

  pstk = (VP)((UW)tcb->stk & ~7UL);
  /* Exception frame: xPSR, PC, LR, R12, R3-R0; then software-saved R11-R4, EXC_RETURN */
  *--pstk = 0x01000000; /* xPSR Thumb */
  *--pstk = (UW)tcb->task | 1;
  *--pstk = (UW)exit_task;
  *--pstk = (UW)0x12121212L;
  *--pstk = (UW)0x03030303L;
  *--pstk = (UW)0x02020202L;
  *--pstk = (UW)0x01010101L;
  *--pstk = (UW)tcb->exinf;
  *--pstk = (UW)0x11111111L;
  *--pstk = (UW)0x10101010L;
  *--pstk = (UW)0x09090909L;
  *--pstk = (UW)0x08080808L;
  *--pstk = (UW)0x07070707L;
  *--pstk = (UW)0x06060606L;
  *--pstk = (UW)0x05050505L;
  *--pstk = (UW)0x04040404L;
  *--pstk = (UW)EXC_RETURN_THREAD_PSP;

  tcb->tskctxb.sp = pstk;
}

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
  NVIC_SetPriority(PendSV_IRQn, 0xFF);
  NVIC_SetPriority(SVCall_IRQn, 0x00);
  NVIC_SetPriority(SysTick_IRQn, 0x80);
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
void __attribute__((weak)) _low_level_init(void)
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

void DebugMon_Handler(void)
{
}

// SysTick irq haldler
__attribute__((naked)) void SysTick_Handler(void)
{
    interrupt_handler(TICKER_VEC_NO);
}

