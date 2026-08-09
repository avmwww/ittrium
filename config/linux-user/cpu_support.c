/**
 * Linux userspace port: ucontext switch + SIGALRM tick / soft IRQ
 */
#include "ittrium.h"
#include "../kernel/task.h"
#include "../cpu_generic.c"

#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

struct ivt_t {
	FP func;
	UW prio;
} int_vector_table[32];

volatile UW irq_count[32];

sigset_t linux_user_irq_sigset;

static ucontext_t idle_ctx;
static uint8_t idle_stack[IDLE_TSK_STACK_SIZE]
	__attribute__((aligned(16)));

static void task_entry(uintptr_t tcb_bits)
{
	TCB *tcb = (TCB *)tcb_bits;
	FP_VP entry = tcb->tskctxb.entry;
	VP arg = tcb->tskctxb.arg;

	cpu_unlock();
	entry(arg);
	ext_tsk();
}

static void idle_entry(void)
{
	for (;;) {
		cpu_unlock();
		while (!schedtsk)
			pause();
		cpu_lock();
		if (schedtsk) {
			runtsk = schedtsk;
			setcontext(&runtsk->tskctxb.uc);
		}
	}
}

void make_task_context(TCB *tcb)
{
	getcontext(&tcb->tskctxb.uc);
	tcb->tskctxb.uc.uc_stack.ss_sp = tcb->stk_base;
	tcb->tskctxb.uc.uc_stack.ss_size = tcb->stksz;
	tcb->tskctxb.uc.uc_link = 0;
	sigemptyset(&tcb->tskctxb.uc.uc_sigmask);
	tcb->tskctxb.entry = (FP_VP)tcb->task;
	tcb->tskctxb.arg = tcb->exinf;
	makecontext(&tcb->tskctxb.uc, (void (*)(void))task_entry, 1,
		    (uintptr_t)tcb);
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

static void deliver_from_signal(INHNO vec, ucontext_t *uc)
{
	(void)uc;

	if (int_nesting)
		return;

	if ((unsigned)vec < 32u)
		irq_count[vec]++;

	int_nesting++;
	if (int_vector_table[vec].func)
		int_vector_table[vec].func();

	if (vec == TICKER_VEC_NO && int_vector_table[UART_VEC_NO].func) {
		irq_count[UART_VEC_NO]++;
		int_vector_table[UART_VEC_NO].func();
	}
	int_nesting--;
	/* No preempt here — signal ucontext + swapcontext is unsafe on Linux */
}

static void linux_user_signal(int sig, siginfo_t *si, void *ucontext)
{
	ucontext_t *uc = ucontext;
	INHNO vec;

	(void)si;

	if (sig == SIGALRM)
		vec = TICKER_VEC_NO;
	else
		vec = UART_VEC_NO;

	deliver_from_signal(vec, uc);
}

void dispatch(void)
{
	sigset_t oldmask;
	TCB *prev;
	TCB *next;

	if (int_nesting)
		return;

	sigprocmask(SIG_BLOCK, &linux_user_irq_sigset, &oldmask);

	next = schedtsk;
	prev = runtsk;
	if (prev == next) {
		sigprocmask(SIG_SETMASK, &oldmask, NULL);
		return;
	}

	if (prev && next) {
		runtsk = next;
		swapcontext(&prev->tskctxb.uc, &next->tskctxb.uc);
	} else if (!prev && next) {
		runtsk = next;
		setcontext(&next->tskctxb.uc);
	} else if (prev && !next) {
		runtsk = NULL;
		swapcontext(&prev->tskctxb.uc, &idle_ctx);
	} else {
		setcontext(&idle_ctx);
	}

	sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

void linux_user_timer_start(void)
{
	struct itimerval it;
	unsigned long us;

	us = (1000000UL * (unsigned long)TIC_NUME) / (unsigned long)TIC_DENO;
	if (us == 0)
		us = 1000;

	memset(&it, 0, sizeof(it));
	it.it_interval.tv_sec = (time_t)(us / 1000000UL);
	it.it_interval.tv_usec = (suseconds_t)(us % 1000000UL);
	it.it_value = it.it_interval;
	setitimer(ITIMER_REAL, &it, NULL);
}

void linux_user_timer_stop(void)
{
	struct itimerval it;

	memset(&it, 0, sizeof(it));
	setitimer(ITIMER_REAL, &it, NULL);
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

void _int_init(void)
{
	struct sigaction sa;
	int i;

	int_nesting = 0;
	for (i = 0; i < 32; i++) {
		int_vector_table[i].func = (FP)0;
		int_vector_table[i].prio = 0;
		irq_count[i] = 0;
	}

	sigemptyset(&linux_user_irq_sigset);
	sigaddset(&linux_user_irq_sigset, SIGALRM);
	sigaddset(&linux_user_irq_sigset, SIGIO);

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = linux_user_signal;
	sa.sa_flags = SA_SIGINFO | SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGALRM);
	sigaddset(&sa.sa_mask, SIGIO);
	sigaction(SIGALRM, &sa, NULL);
	sigaction(SIGIO, &sa, NULL);

	getcontext(&idle_ctx);
	idle_ctx.uc_stack.ss_sp = idle_stack;
	idle_ctx.uc_stack.ss_size = sizeof(idle_stack);
	idle_ctx.uc_link = 0;
	sigemptyset(&idle_ctx.uc_sigmask);
	makecontext(&idle_ctx, idle_entry, 0);

	/* Block SIGALRM/SIGIO until first task_entry unlocks */
	cpu_lock();
}

void __attribute__((weak)) _low_level_init(void)
{
}

void low_level_init(void)
{
	_int_init();
	_low_level_init();
}

const char *irq_vec_name(unsigned vec)
{
	switch (vec) {
	case TICKER_VEC_NO: return "timer";
	case UART_VEC_NO: return "uart";
	case NETDEV_VEC_NO: return "netdev";
	default: return "?";
	}
}
