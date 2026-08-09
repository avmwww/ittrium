#ifndef LM3S6965_H_
#define LM3S6965_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	NonMaskableInt_IRQn = -14,
	HardFault_IRQn      = -13,
	MemoryManagement_IRQn = -12,
	BusFault_IRQn       = -11,
	UsageFault_IRQn     = -10,
	SVCall_IRQn         = -5,
	DebugMonitor_IRQn   = -4,
	PendSV_IRQn         = -2,
	SysTick_IRQn        = -1,
	GPIOA_IRQn          = 0,
	GPIOB_IRQn          = 1,
	GPIOC_IRQn          = 2,
	GPIOD_IRQn          = 3,
	GPIOE_IRQn          = 4,
	UART0_IRQn          = 5,
	UART1_IRQn          = 6,
	SSI0_IRQn           = 7,
	I2C0_IRQn           = 8,
	PWM0_FAULT_IRQn     = 9,
	PWM0_0_IRQn         = 10,
	PWM0_1_IRQn         = 11,
	PWM0_2_IRQn         = 12,
	QEI0_IRQn           = 13,
	ADC0SS0_IRQn        = 14,
	ADC0SS1_IRQn        = 15,
	ADC0SS2_IRQn        = 16,
	ADC0SS3_IRQn        = 17,
	WATCHDOG0_IRQn      = 18,
	TIMER0A_IRQn        = 19,
	TIMER0B_IRQn        = 20,
	TIMER1A_IRQn        = 21,
	TIMER1B_IRQn        = 22,
	TIMER2A_IRQn        = 23,
	TIMER2B_IRQn        = 24,
	COMP0_IRQn          = 25,
	COMP1_IRQn          = 26,
	COMP2_IRQn          = 27,
	SYSCTL_IRQn         = 28,
	FLASH_CTRL_IRQn     = 29,
	GPIOF_IRQn          = 30,
	GPIOG_IRQn          = 31,
} IRQn_Type;

#define __CM3_REV                 0x0200U
#define __MPU_PRESENT             1U
#define __NVIC_PRIO_BITS          3U
#define __Vendor_SysTickConfig    0U

#include "core_cm3.h"

#define UART0_BASE        0x4000C000UL
#define SYSCTL_BASE       0x400FE000UL

#define SYSCTL_RCGC1      (*(volatile uint32_t *)(SYSCTL_BASE + 0x104))
#define SYSCTL_RCGC1_UART0 (1u << 0)

#define UART_DR(base)     (*(volatile uint32_t *)((base) + 0x000))
#define UART_FR(base)     (*(volatile uint32_t *)((base) + 0x018))
#define UART_IBRD(base)   (*(volatile uint32_t *)((base) + 0x024))
#define UART_FBRD(base)   (*(volatile uint32_t *)((base) + 0x028))
#define UART_LCRH(base)   (*(volatile uint32_t *)((base) + 0x02C))
#define UART_CTL(base)    (*(volatile uint32_t *)((base) + 0x030))
#define UART_IMSC(base)   (*(volatile uint32_t *)((base) + 0x038))
#define UART_MIS(base)    (*(volatile uint32_t *)((base) + 0x040))
#define UART_ICR(base)    (*(volatile uint32_t *)((base) + 0x044))

#define UART_FR_RXFE      (1u << 4)
#define UART_FR_TXFF      (1u << 5)
#define UART_LCRH_WLEN_8  (3u << 5)
#define UART_LCRH_FEN     (1u << 4)
#define UART_CTL_UARTEN   (1u << 0)
#define UART_CTL_TXE      (1u << 8)
#define UART_CTL_RXE      (1u << 9)
#define UART_INT_RX       (1u << 4)
#define UART_INT_TX       (1u << 5)
#define UART_INT_RT       (1u << 6)

extern uint32_t SystemCoreClock;

#ifdef __cplusplus
}
#endif

#endif
