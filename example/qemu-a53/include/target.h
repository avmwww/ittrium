#ifndef _TARGET_H_
#define _TARGET_H_

#include <stdint.h>

/* QEMU virt machine (gic-version=2) */
#define GICD_BASE       0x08000000UL
#define GICC_BASE       0x08010000UL
#define UART_BASE       0x09000000UL
#define TIMER_IRQ       30u
/* PL011: DTB SPI 1 → GIC ID 32+1 */
#define UART_IRQ        33u
#define UART_IRQ_PRIO   0xa0u

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
int uart_getc(void);

#endif /* _TARGET_H_ */
