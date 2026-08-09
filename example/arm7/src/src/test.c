#include <stdio.h>
#include "ittrium.h"


#define SIMUL_DIV    100

unsigned long init_tsk_stack[INIT_TASK_STACK_SIZE];

unsigned long test_tsk_stack[TEST_TSK_STACK_SIZE];


void test_tsk(void *exinf)
{
  FLGPTN flgptn;

  (void)exinf;
  for (;;) {
    if (E_OK == twai_flg(TEST_FLG_ID, 0x0001, TWF_ORW, &flgptn, 1000 / SIMUL_DIV))
      printf("flag ready\n");
    else
      printf("flag timeout\n");
  }
}

void init_tsk(void *exinf)
{
  T_CTSK pk_ctsk;
  T_CFLG pk_cflg;

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


  cre_tsk(TEST_TASK_ID, &pk_ctsk);
  act_tsk(TEST_TASK_ID);
  chg_pri(TSK_SELF, LOW_PRIO);

  for (;;) {
    dly_tsk(2500 / SIMUL_DIV);
    set_flg(TEST_FLG_ID, 0x0001);
  }
}

void _low_level_init(void)
{
  /* clocks / VIC as needed for the board */
}
