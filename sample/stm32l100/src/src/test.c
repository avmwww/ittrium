#include <stdio.h>
#include "ittrium.h"

/* Поправка таймера на сработу с симулятором  */
#define SIMUL_DIV    1

/* Вектор какого-то прерывания: Software #30 */
#define TEST_INT_VEC_NO  2

/**
 * Стек под init_tsk()
 */
short init_tsk_stack[INIT_TASK_STACK_SIZE];

/**
 * Стек под test_task()
 */
short test_tsk_stack[TEST_TSK_STACK_SIZE];


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
  FLGPTN flgptn = 1;

  for (;;) {
    /* Ожидание с таймаутом 15 мс */
    if (E_OK == twai_flg(TEST_FLG_ID, flgptn, TWF_ORW, &flgptn, 1000/SIMUL_DIV)) {
      /* Дождались флага */
      printf("Flag is ready\n");
    } else {
     /* Ошибка, скорее все таймаут = E_TMO */
      printf("Flag timeout\n");
    }
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
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

  /* Создаем задачу */
  cre_tsk(TEST_TASK_ID, &pk_ctsk);
  /* Активируем задачу */
  act_tsk(TEST_TASK_ID);

  /* Устанавливаем обработчик прерывания */
  install_handler(test_handler, TEST_INT_VEC_NO, 1);
  chg_pri(TSK_SELF, LOW_PRIO);

  for (;;) {
    dly_tsk(2500/SIMUL_DIV);
  }
}

/**
 * инициализируем периферию процессора
 */
static void Error_Handler(void)
{
}

void target_clock_init(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Configure the main internal regulator output voltage */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
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

    /* Initializes the CPU, AHB and APB buses clocks */
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

