/**
 * @brief Cortex-A53 AArch64 EL1 (no MMU) port
 */
#include "ittrium.h"
#include "../kernel/task.h"
#include "../cpu_generic.c"

/* Context layout must match cpu_support.S / ittrium.inc */
#define CTX_FRAME_SIZE  272
#define CTX_OFF_X0      0
#define CTX_OFF_X30     240
#define CTX_OFF_SPSR    248
#define CTX_OFF_ELR     256

/* EL1h, AArch64, IRQs enabled (I=0) */
#define SPSR_EL1H       0x00000005ULL

struct ivt_t {
	FP func;
	UW prio;
} int_vector_table[32];

uint64_t idle_stack[IDLE_TSK_STACK_SIZE / sizeof(uint64_t)];

extern void vectors_start(void);

static void exit_task(void)
{
	for (;;)
		cpu_wait();
}

void make_task_context(TCB *tcb)
{
	UD *pstk;
	int i;

	pstk = (UD *)(((uintptr_t)tcb->stk) & ~15ULL);
	pstk = (UD *)((uintptr_t)pstk - CTX_FRAME_SIZE);

	for (i = 0; i < 31; i++)
		pstk[i] = 0;

	pstk[0] = (UD)(uintptr_t)tcb->exinf; /* x0 */
	pstk[CTX_OFF_X30 / 8] = (UD)(uintptr_t)exit_task;
	pstk[CTX_OFF_SPSR / 8] = SPSR_EL1H;
	pstk[CTX_OFF_ELR / 8] = (UD)(uintptr_t)tcb->task;

	tcb->tskctxb.sp = pstk;
	tcb->tskctxb.pc = (VP)tcb->task;
}

void install_handler(FP handler, INHNO vec_no, UB prio)
{
	if (vec_no < 32) {
		BEGIN_CRITICAL_SECTION;
		int_vector_table[vec_no].func = handler;
		int_vector_table[vec_no].prio = prio;
		END_CRITICAL_SECTION;
	}
}

void _int_init(void)
{
	int i;

	int_nesting = 0;
	for (i = 0; i < 32; i++) {
		int_vector_table[i].func = (FP)0;
		int_vector_table[i].prio = 0;
	}

	__asm__ volatile("msr vbar_el1, %0" :: "r"(vectors_start) : "memory");
	gic_init();
}

void timer_hw_program(void)
{
	uint64_t freq;
	uint64_t ticks;

	__asm__ volatile("mrs %0, cntfrq_el0" : "=r"(freq));
	/* 1 ms tick: TIC_NUME/TIC_DENO relative to seconds */
	ticks = (freq * (uint64_t)TIC_NUME) / (uint64_t)TIC_DENO;
	if (ticks == 0)
		ticks = freq / 1000;
	__asm__ volatile("msr cntp_tval_el0, %0" :: "r"(ticks));
}

void start_hw_timer(void)
{
	timer_nesting = 0;
	INIT_SYSTEM_TICKS();

	install_handler(timer_handler, TICKER_VEC_NO, TICKER_PRIO);
	ENABLE_TICKER_INT();
}

void terminate_hw_timer(void)
{
	DISABLE_TICKER_INT();
	int_vector_table[TICKER_VEC_NO].func = (FP)0;
	int_vector_table[TICKER_VEC_NO].prio = 0;
}

void __attribute__((weak)) _low_level_init(void)
{
}

void low_level_init(void)
{
	_int_init();
	_low_level_init();
}

/*
 * Called from IRQ asm stub with IAR value.
 * Soft-vector for timer PPI; EOI done in asm after return.
 */
void irq_handle(uint32_t iar)
{
	uint32_t irq = iar & 0x3ffu;
	INHNO vec;

	if (irq >= 1020u)
		return;

	if (irq == TIMER_IRQ)
		vec = TICKER_VEC_NO;
	else if (irq == UART_IRQ)
		vec = UART_VEC_NO;
	else if (irq < 32u && int_vector_table[irq].func)
		vec = (INHNO)irq;
	else
		return;

	int_nesting++;
	if (int_vector_table[vec].func)
		int_vector_table[vec].func();
	int_nesting--;
}

/* ---- GICv2 (bases from target.h) ---- */

#define GICD_CTLR           (*(volatile uint32_t *)(GICD_BASE + 0x000))
#define GICD_ISENABLER(n)   (*(volatile uint32_t *)(GICD_BASE + 0x100 + 4u * (n)))
#define GICD_ICENABLER(n)   (*(volatile uint32_t *)(GICD_BASE + 0x180 + 4u * (n)))
#define GICD_IGROUPR(n)     (*(volatile uint32_t *)(GICD_BASE + 0x080 + 4u * (n)))
#define GICD_IPRIORITYR(n)  (*(volatile uint8_t  *)(GICD_BASE + 0x400 + (n)))
#define GICD_ITARGETSR(n)   (*(volatile uint8_t  *)(GICD_BASE + 0x800 + (n)))
#define GICD_ICFGR(n)       (*(volatile uint32_t *)(GICD_BASE + 0xC00 + 4u * (n)))

#define GICC_CTLR           (*(volatile uint32_t *)(GICC_BASE + 0x000))
#define GICC_PMR            (*(volatile uint32_t *)(GICC_BASE + 0x004))
#define GICC_BPR            (*(volatile uint32_t *)(GICC_BASE + 0x008))
#define GICC_IAR            (*(volatile uint32_t *)(GICC_BASE + 0x00C))
#define GICC_EOIR           (*(volatile uint32_t *)(GICC_BASE + 0x010))

void gic_init(void)
{
	GICD_CTLR = 0;
	GICC_CTLR = 0;

	GICC_PMR = 0xff;
	GICC_BPR = 0;
	/* Group 0 only — matches QEMU virt EL1 bring-up */
	GICC_CTLR = 1;
	GICD_CTLR = 1;
}

void gic_enable_irq(unsigned int irq, UB prio)
{
	unsigned int reg = irq / 32u;
	unsigned int bit = irq % 32u;

	GICD_IPRIORITYR(irq) = prio;
	/* Group 0 */
	GICD_IGROUPR(reg) &= ~(1u << bit);
	if (irq >= 32u)
		GICD_ITARGETSR(irq) = 0x01;
	GICD_ISENABLER(reg) = (1u << bit);
}

uint32_t gic_acknowledge(void)
{
	return GICC_IAR;
}

void gic_eoi(uint32_t iar)
{
	GICC_EOIR = iar;
}
