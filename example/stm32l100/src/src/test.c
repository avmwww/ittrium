#include <stdio.h>
#include "ittrium.h"

/* Scale delays for simulator vs hardware (1 = real time) */
#define SIMUL_DIV    1

/* Soft IRQ vector used by the demo (see install_handler) */
#define TEST_INT_VEC_NO  2

short init_tsk_stack[INIT_TASK_STACK_SIZE];
short test_tsk_stack[TEST_TSK_STACK_SIZE];

void test_handler(void)
{
  iset_flg (TEST_FLG_ID, 0x1234);
}

void test_tsk(void *exinf)
{
  FLGPTN flgptn = 1;

  for (;;) {
    if (E_OK == twai_flg(TEST_FLG_ID, flgptn, TWF_ORW, &flgptn, 1000/SIMUL_DIV)) {
      printf("Flag is ready\n");
    } else {
      printf("Flag timeout\n");
    }
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
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

  cre_tsk(TEST_TASK_ID, &pk_ctsk);
  act_tsk(TEST_TASK_ID);

  install_handler(test_handler, TEST_INT_VEC_NO, 1);
  chg_pri(TSK_SELF, LOW_PRIO);

  for (;;) {
    dly_tsk(2500/SIMUL_DIV);
  }
}

static void Error_Handler(void)
{
}

void target_clock_init(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
        |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

static void target_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_Init;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_Init.Pin = GPIO_PIN_12;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_Init.Alternate = 0;
    HAL_GPIO_Init(GPIOB, &GPIO_Init);
}


void _low_level_init(void)
{
    HAL_Init();

    //__HAL_RCC_PWR_CLK_ENABLE();
    target_clock_init();
    target_gpio_init();
}
