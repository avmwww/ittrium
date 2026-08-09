#include "ittrium.h"
#include "target.h"
#include "drv/console.h"

#define UART_FIFO_SIZE 128
#define UART_FLG_RX    (1u << 0)
#define UART_FLG_TX    (1u << 1)
#define UART_BAUD      115200u

static uint8_t rx_fifo[UART_FIFO_SIZE];
static uint8_t tx_fifo[UART_FIFO_SIZE];
static volatile unsigned rx_head, rx_tail, rx_count;
static volatile unsigned tx_head, tx_tail, tx_count;
static int uart_ready;
static int uart_hw_ready;

static const struct console_ops g_console_ops = {
	.putc = uart_putc,
	.getc = uart_getc,
};

static void rx_push(uint8_t b)
{
	if (rx_count >= UART_FIFO_SIZE)
		return;
	rx_fifo[rx_head] = b;
	rx_head = (rx_head + 1u) % UART_FIFO_SIZE;
	rx_count++;
}

static int rx_pop(uint8_t *b)
{
	if (rx_count == 0)
		return 0;
	*b = rx_fifo[rx_tail];
	rx_tail = (rx_tail + 1u) % UART_FIFO_SIZE;
	rx_count--;
	return 1;
}

static int tx_push(uint8_t b)
{
	if (tx_count >= UART_FIFO_SIZE)
		return 0;
	tx_fifo[tx_head] = b;
	tx_head = (tx_head + 1u) % UART_FIFO_SIZE;
	tx_count++;
	return 1;
}

static int tx_pop(uint8_t *b)
{
	if (tx_count == 0)
		return 0;
	*b = tx_fifo[tx_tail];
	tx_tail = (tx_tail + 1u) % UART_FIFO_SIZE;
	tx_count--;
	return 1;
}

static void uart_tx_kick(void)
{
	uint8_t b;

	while ((USART1->SR & USART_SR_TXE) && tx_pop(&b))
		USART1->DR = b;

	if (tx_count)
		USART1->CR1 |= USART_CR1_TXEIE;
	else
		USART1->CR1 &= ~USART_CR1_TXEIE;
}

static void uart_regs_init(void)
{
	uint32_t pclk;
	uint32_t div;

	if (uart_hw_ready)
		return;

	__HAL_RCC_USART1_CLK_ENABLE();

	USART1->CR1 = 0;
	USART1->CR2 = 0;
	USART1->CR3 = 0;

	pclk = HAL_RCC_GetPCLK2Freq();
	if (pclk == 0)
		pclk = SystemCoreClock;
	div = (pclk + (UART_BAUD / 2u)) / UART_BAUD;
	USART1->BRR = div;

	USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
	uart_hw_ready = 1;
}

static void uart_irq(void)
{
	uint32_t sr = USART1->SR;
	FLGPTN set = 0;

	if (sr & USART_SR_RXNE) {
		while (USART1->SR & USART_SR_RXNE)
			rx_push((uint8_t)(USART1->DR & 0xffu));
		if (rx_count)
			set |= UART_FLG_RX;
	}

	if (sr & USART_SR_TXE) {
		uart_tx_kick();
		if (tx_count < UART_FIFO_SIZE)
			set |= UART_FLG_TX;
	}

	if (set)
		iset_flg(UART_FLG_ID, set);
}

void USART1_IRQHandler(void) __attribute__((naked));
void USART1_IRQHandler(void)
{
	interrupt_handler(UART_VEC_NO);
}

void uart_init(void)
{
	T_CFLG pk_cflg;

	rx_head = rx_tail = rx_count = 0;
	tx_head = tx_tail = tx_count = 0;

	uart_regs_init();

	pk_cflg.flgatr = TA_TFIFO | TA_CLR | TA_WMUL;
	pk_cflg.iflgptn = UART_FLG_TX;
	cre_flg(UART_FLG_ID, &pk_cflg);

	install_handler(uart_irq, UART_VEC_NO, UART_IRQ_PRIO);
	NVIC_SetPriority(USART1_IRQn, UART_IRQ_PRIO);
	NVIC_EnableIRQ(USART1_IRQn);

	USART1->CR1 |= USART_CR1_RXNEIE;
	uart_ready = 1;
	console_register(&g_console_ops);
}

void uart_putc(char c)
{
	lock_state_t ls;
	FLGPTN ptn;

	if (c == '\n')
		uart_putc('\r');

	uart_regs_init();

	if (!uart_ready) {
		while (!(USART1->SR & USART_SR_TXE))
			;
		USART1->DR = (uint32_t)(uint8_t)c;
		return;
	}

	for (;;) {
		begin_critical_section(ls);
		if (tx_push((uint8_t)c)) {
			uart_tx_kick();
			end_critical_section(ls);
			return;
		}
		end_critical_section(ls);
		twai_flg(UART_FLG_ID, UART_FLG_TX, TWF_ORW, &ptn, TMO_FEVR);
	}
}

int uart_getc(void)
{
	lock_state_t ls;
	FLGPTN ptn;
	uint8_t b;

	for (;;) {
		begin_critical_section(ls);
		if (rx_pop(&b)) {
			end_critical_section(ls);
			return (int)b;
		}
		end_critical_section(ls);
		twai_flg(UART_FLG_ID, UART_FLG_RX, TWF_ORW, &ptn, TMO_FEVR);
	}
}

int __io_putchar(int ch)
{
	uart_putc((char)ch);
	return ch;
}
