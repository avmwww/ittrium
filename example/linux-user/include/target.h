#ifndef _TARGET_H_
#define _TARGET_H_

#include <stdint.h>

#define UART_IRQ_PRIO   0xa0u

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
int uart_getc(void);

#endif /* _TARGET_H_ */
