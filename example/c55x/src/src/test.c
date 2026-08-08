#include <stdio.h>
#include "ittrium.h"

/* Scale delays for CCS simulator vs hardware */
#define SIMUL_DIV    100

/* Software interrupt #30 - fired from init_tsk via INTR */
#define TEST_INT_VEC_NO  SINT30_VECNO

short init_tsk_stack[INIT_TASK_STACK_SIZE];
short init_tsk_sys_stack[INIT_TASK_STACK_SIZE];

short test_tsk_stack[TEST_TSK_STACK_SIZE];
short test_tsk_sys_stack[TEST_TSK_STACK_SIZE];

void test_handler(void)
{
  iset_flg (TEST_FLG_ID, 0x1234);
}

void test_tsk(void *exinf)
{
  FLGPTN flgptn;

  for (;;) {
    if (E_OK == twai_flg(TEST_FLG_ID, flgptn, TWF_ORW, &flgptn, 1000/SIMUL_DIV)) {
      printf("Flag is ready\n");
    } else {
      printf("Flag timeout\n");
    }
  }
}


void init_tsk(void *exinf)
{
  T_CTSK pk_ctsk;
  T_CFLG pk_cflg;

  pk_cflg.flgatr = TA_TFIFO|TA_CLR|TA_WMUL;
  pk_cflg.iflgptn = 0;
  cre_flg(TEST_FLG_ID, &pk_cflg);

  pk_ctsk.tskatr = TA_HLNG;
  pk_ctsk.exinf = 0;
  pk_ctsk.task = (FP)test_tsk;
  pk_ctsk.itskpri = TEST_TASK_PRIO;
  pk_ctsk.stksz = TEST_TSK_STACK_SIZE;
  pk_ctsk.stk = test_tsk_stack;
  pk_ctsk.sstk = test_tsk_sys_stack;

  cre_tsk(TEST_TASK_ID, &pk_ctsk);
  act_tsk(TEST_TASK_ID);

  install_handler(test_handler, TEST_INT_VEC_NO);
  chg_pri(TSK_SELF, LOW_PRIO);

  for (;;) {
    dly_tsk(2500/SIMUL_DIV);
    asm("   INTR #30");
  }
}

void _low_level_init(void)
{
  /* CPU clock = 7.68 MHz * 18 = 138.240 MHz */
  CLKMD = CLKMD_PLL_ENABLE|(18<<_CLKMD_PLL_MULT_)|(0<<_CLKMD_PLL_DIV_);
}
