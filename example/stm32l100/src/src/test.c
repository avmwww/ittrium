#include "ittrium.h"
#include "target.h"
#include "drv/console.h"

#if CFG_USE_SHELL
#include "shell.h"
#endif
#if CFG_USE_VFS
#include "vfs.h"
#endif
#if CFG_USE_ROMFS
#include "romfs.h"
#endif
#if CFG_USE_PROCFS
#include "procfs.h"
#endif

uint32_t init_tsk_stack[INIT_TASK_STACK_SIZE / sizeof(uint32_t)];
#if CFG_USE_SHELL
uint32_t shell_tsk_stack[SHELL_TSK_STACK_SIZE / sizeof(uint32_t)];
#endif

#if CFG_USE_SHELL
static void start_shell(void)
{
	T_CTSK pk;

	shell_set_io(console_putc, console_getc);

	pk.tskatr = TA_HLNG | TA_NAME;
	pk.exinf = 0;
	pk.task = (FP)shell_task;
	pk.itskpri = SHELL_TASK_PRIO;
	pk.stksz = SHELL_TSK_STACK_SIZE;
	pk.stk = shell_tsk_stack;
	pk.name = "shell";

	cre_tsk(SHELL_TASK_ID, &pk);
	act_tsk(SHELL_TASK_ID);
}
#endif

void init_tsk(void *exinf)
{
	(void)exinf;

	uart_init();
	console_puts("ittrium stm32l100\n");

#if CFG_USE_VFS
	console_puts("vfs...\n");
	vfs_init();
#endif
#if CFG_USE_ROMFS
	console_puts("romfs...\n");
	{
		static const char demo_sh[] =
			"# ittrium demo\n"
			"echo hello from demo.sh\n"
			"set DEMO=1\n"
			"echo DEMO=$DEMO\n";
		if (romfs_add_builtin("demo.sh", demo_sh, sizeof(demo_sh) - 1) != 0)
			console_puts("romfs: demo.sh add failed\n");
	}
	romfs_mount("/", NULL, 0);
#endif
#if CFG_USE_PROCFS
	console_puts("proc...\n");
	procfs_mount("/proc");
#endif

#if CFG_USE_SHELL
	start_shell();
#endif
	chg_pri(TSK_SELF, LOW_PRIO);

	for (;;)
		dly_tsk(10000);
}

static void target_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_Init = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_Init.Pin = GPIO_PIN_12;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_Init);
}

void _low_level_init(void)
{
	/* HSI @ 16 MHz after reset. Skip HAL_RCC — its timeouts need HAL tick,
	 * while SysTick is owned by ittrium. */
	SystemCoreClock = 16000000u;
	SysTick_Config(SystemCoreClock / TIC_DENO);
	NVIC_SetPriority(SysTick_IRQn, 0x80);
	target_gpio_init();
}
