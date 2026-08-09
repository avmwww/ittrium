#include <stdint.h>
#include "lm3s6965.h"

extern uint32_t _sdata, _edata, _sidata, _sbss, _ebss, _estack;
extern int main(int argc, char **argv);

void Reset_Handler(void);
void Default_Handler(void);

void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));

void GPIOA_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void GPIOB_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void GPIOC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void GPIOD_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void GPIOE_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void UART0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void UART1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SSI0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIMER0A_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));

uint32_t SystemCoreClock = 50000000UL;

void Default_Handler(void)
{
	for (;;)
		;
}

void Reset_Handler(void)
{
	uint32_t *src = &_sidata;
	uint32_t *dst = &_sdata;

	while (dst < &_edata)
		*dst++ = *src++;
	for (dst = &_sbss; dst < &_ebss; dst++)
		*dst = 0;

	main(0, (char **)0);
	for (;;)
		;
}

__attribute__((section(".isr_vector"), used))
const uint32_t g_pfnVectors[] = {
	(uint32_t)&_estack,
	(uint32_t)Reset_Handler,
	(uint32_t)NMI_Handler,
	(uint32_t)HardFault_Handler,
	(uint32_t)MemManage_Handler,
	(uint32_t)BusFault_Handler,
	(uint32_t)UsageFault_Handler,
	0, 0, 0, 0,
	(uint32_t)SVC_Handler,
	(uint32_t)DebugMon_Handler,
	0,
	(uint32_t)PendSV_Handler,
	(uint32_t)SysTick_Handler,
	(uint32_t)GPIOA_IRQHandler,
	(uint32_t)GPIOB_IRQHandler,
	(uint32_t)GPIOC_IRQHandler,
	(uint32_t)GPIOD_IRQHandler,
	(uint32_t)GPIOE_IRQHandler,
	(uint32_t)UART0_IRQHandler,
	(uint32_t)UART1_IRQHandler,
	(uint32_t)SSI0_IRQHandler,
	(uint32_t)I2C0_IRQHandler,
	(uint32_t)Default_Handler,
	(uint32_t)Default_Handler,
	(uint32_t)Default_Handler,
	(uint32_t)Default_Handler,
	(uint32_t)Default_Handler,
	(uint32_t)Default_Handler,
	(uint32_t)Default_Handler,
	(uint32_t)Default_Handler,
	(uint32_t)Default_Handler,
	(uint32_t)Default_Handler,
	(uint32_t)TIMER0A_IRQHandler,
};
