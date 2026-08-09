#include "target.h"

void uart_init(void)
{
	SYSCTL_RCGC1 |= SYSCTL_RCGC1_UART0;

	UART_CTL(UART0_BASE) = 0;
	/* 115200 8N1 @ 50 MHz: IBRD=27, FBRD=8 */
	UART_IBRD(UART0_BASE) = 27;
	UART_FBRD(UART0_BASE) = 8;
	UART_LCRH(UART0_BASE) = UART_LCRH_WLEN_8 | UART_LCRH_FEN;
	UART_CTL(UART0_BASE) = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;
}

void uart_putc(char c)
{
	if (c == '\n')
		uart_putc('\r');
	while (UART_FR(UART0_BASE) & UART_FR_TXFF)
		;
	UART_DR(UART0_BASE) = (uint32_t)(uint8_t)c;
}

int __io_putchar(int ch)
{
	uart_putc((char)ch);
	return ch;
}
