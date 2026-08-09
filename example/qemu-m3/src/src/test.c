#include <stdio.h>
#include "ittrium.h"

#define TEST_INT_VEC_NO  2

short init_tsk_stack[INIT_TASK_STACK_SIZE / sizeof(short)];
short test_tsk_stack[TEST_TSK_STACK_SIZE / sizeof(short)];

void uart_init(void);

void test_handler(void)
{
	iset_flg(TEST_FLG_ID, 0x1234);
}

__attribute__((naked)) void GPIOA_IRQHandler(void)
{
	interrupt_handler(TEST_INT_VEC_NO);
}

void test_tsk(void *exinf)
{
	FLGPTN flgptn = 0x1234;
	unsigned n = 0;

	(void)exinf;
	for (;;) {
		if (E_OK == twai_flg(TEST_FLG_ID, flgptn, TWF_ORW, &flgptn, 1000)) {
			printf("flag ready #%u\n", n);
		} else {
			printf("flag timeout #%u\n", n);
		}
		n++;
		if (n >= 4) {
			printf("qemu-m3 smoke ok\n");
			for (;;)
				cpu_wait();
		}
	}
}

void init_tsk(void *exinf)
{
	T_CTSK pk_ctsk;
	T_CFLG pk_cflg;
	unsigned kick = 0;

	(void)exinf;

	pk_cflg.flgatr = TA_TFIFO | TA_CLR | TA_WMUL;
	pk_cflg.iflgptn = 0;
	cre_flg(TEST_FLG_ID, &pk_cflg);

	pk_ctsk.tskatr = TA_HLNG;
	pk_ctsk.exinf = 0;
	pk_ctsk.task = (FP)test_tsk;
	pk_ctsk.itskpri = TEST_TASK_PRIO;
	pk_ctsk.stksz = TEST_TSK_STACK_SIZE;
	pk_ctsk.stk = test_tsk_stack;
	pk_ctsk.name = 0;

	cre_tsk(TEST_TASK_ID, &pk_ctsk);
	act_tsk(TEST_TASK_ID);

	install_handler(test_handler, TEST_INT_VEC_NO, 1);
	NVIC_SetPriority(GPIOA_IRQn, 0x40);
	NVIC_EnableIRQ(GPIOA_IRQn);

	chg_pri(TSK_SELF, LOW_PRIO);

	for (;;) {
		dly_tsk(500);
		if ((kick++ & 1u) == 0u)
			NVIC_SetPendingIRQ(GPIOA_IRQn);
	}
}

void _low_level_init(void)
{
	uart_init();
	printf("ittrium qemu-m3 (lm3s6965evb)\n");

	SysTick_Config(SystemCoreClock / TIC_DENO);
	NVIC_SetPriority(SysTick_IRQn, 0x80);
}
