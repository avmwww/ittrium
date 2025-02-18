#include <stdio.h>
#include "ittrium.h"

/* Поправка таймера на сработу с симулятором  */
#define SIMUL_DIV    100

/* Вектор какого-то прерывания: Software #30 */
#define TEST_INT_VEC_NO  SINT30_VECNO

/**
 * Стек под init_tsk()
 */
short init_tsk_stack[INIT_TASK_STACK_SIZE];
short init_tsk_sys_stack[INIT_TASK_STACK_SIZE];

/**
 * Стек под test_task()
 */
short test_tsk_stack[TEST_TSK_STACK_SIZE];
short test_tsk_sys_stack[TEST_TSK_STACK_SIZE];


/**
 * Обработчик прерывания
 */
void test_handler(void)
{
  /* Из прерывания ставим флаг */
  iset_flg (TEST_FLG_ID, 0x1234);
}

/**
 * Тестовая задача
 */
void test_tsk(void *exinf)
{
  FLGPTN flgptn;

  for (;;) {
    /* Ожидание с таймаутом 15 мс */
    if (E_OK == twai_flg(TEST_FLG_ID, flgptn, TWF_ORW, &flgptn, 1000/SIMUL_DIV)) {
      /* Дождались флага */
      printf("Flag is ready\n");
    } else {
     /* Ошибка, скорее все таймаут = E_TMO */
      printf("Flag timeout\n");
    }
  }
}


void init_tsk(void *exinf)
{
  T_CTSK pk_ctsk;
  T_CFLG pk_cflg;

  /* Параметры создаваемого флага:
     очередь ожидания в FIFO, самоочищающийся флаг,
     могут ждать несколько процессов */
  pk_cflg.flgatr = TA_TFIFO|TA_CLR|TA_WMUL;
  /* Начальное значение флага */
  pk_cflg.iflgptn = 0;
  cre_flg(TEST_FLG_ID, &pk_cflg);

  pk_ctsk.tskatr = TA_HLNG;
  pk_ctsk.exinf = 0;
  pk_ctsk.task = (FP)test_tsk;
  pk_ctsk.itskpri = TEST_TASK_PRIO;
  pk_ctsk.stksz = TEST_TSK_STACK_SIZE;
  pk_ctsk.stk = test_tsk_stack;
  pk_ctsk.sstk = test_tsk_sys_stack;


  /* Создаем задачу */
  cre_tsk(TEST_TASK_ID, &pk_ctsk);
  /* Активируем задачу */
  act_tsk(TEST_TASK_ID);

  /* Устанавливаем обработчик прерывания */
  install_handler(test_handler, TEST_INT_VEC_NO);
  chg_pri(TSK_SELF, LOW_PRIO);

  for (;;) {
    /* Эмулируем прерывание раз в 2.5 сек */
    dly_tsk(2500/SIMUL_DIV);
    asm("   INTR #30");
  }
}

/**
 * инициализируем периферию процессора
 */
void _low_level_init(void)
{
  //CPU clock = 7.68MHz*18 = 138.240 MHz
  CLKMD = CLKMD_PLL_ENABLE|(18<<_CLKMD_PLL_MULT_)|(0<<_CLKMD_PLL_DIV_);
}

