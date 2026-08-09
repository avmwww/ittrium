#ifndef _TARGET_H_
#define _TARGET_H_

#include "stm32l1xx.h"
#include "stm32l1xx_hal.h"

#define UART_IRQ_PRIO   0x40u

void uart_init(void);
void uart_putc(char c);
int uart_getc(void);

#endif
