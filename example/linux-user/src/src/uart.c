/**
 * stdin/stdout console (soft FIFO + eventflag)
 */
#include "ittrium.h"
#include "target.h"
#include "drv/console.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define UART_FIFO_SIZE 256

#define UART_FLG_RX   (1u << 0)
#define UART_FLG_TX   (1u << 1)

static uint8_t rx_fifo[UART_FIFO_SIZE];
static volatile unsigned rx_head, rx_tail, rx_count;
static int uart_ready;
static struct termios term_saved;
static int term_raw;

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

static void uart_poll_rx(void)
{
	unsigned char buf[64];
	ssize_t n;
	FLGPTN set = 0;
	int i;

	for (;;) {
		n = read(STDIN_FILENO, buf, sizeof(buf));
		if (n < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			break;
		}
		if (n == 0)
			break;
		for (i = 0; i < (int)n; i++)
			rx_push(buf[i]);
		set |= UART_FLG_RX;
	}

	if (set)
		iset_flg(UART_FLG_ID, set);
}

static void uart_irq(void)
{
	uart_poll_rx();
}

static void uart_term_restore(void)
{
	if (term_raw) {
		tcsetattr(STDIN_FILENO, TCSANOW, &term_saved);
		term_raw = 0;
	}
}

static void uart_on_fatal_signal(int sig)
{
	uart_term_restore();
	signal(sig, SIG_DFL);
	raise(sig);
}

static const struct console_ops g_console_ops = {
	.putc = uart_putc,
	.getc = uart_getc,
};

void uart_init(void)
{
	T_CFLG pk_cflg;
	struct termios t;
	int flags;

	rx_head = rx_tail = rx_count = 0;

	pk_cflg.flgatr = TA_TFIFO | TA_CLR | TA_WMUL;
	pk_cflg.iflgptn = UART_FLG_TX;
	cre_flg(UART_FLG_ID, &pk_cflg);

	if (isatty(STDIN_FILENO) && tcgetattr(STDIN_FILENO, &term_saved) == 0) {
		t = term_saved;
		cfmakeraw(&t);
		t.c_lflag |= ISIG; /* Ctrl+C / Ctrl+\ */
		t.c_cc[VMIN] = 0;
		t.c_cc[VTIME] = 0;
		if (tcsetattr(STDIN_FILENO, TCSANOW, &t) == 0) {
			term_raw = 1;
			atexit(uart_term_restore);
			signal(SIGINT, uart_on_fatal_signal);
			signal(SIGTERM, uart_on_fatal_signal);
			signal(SIGQUIT, uart_on_fatal_signal);
		}
	}

	flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	if (flags >= 0)
		fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK | O_ASYNC);
	fcntl(STDIN_FILENO, F_SETOWN, getpid());

	install_handler(uart_irq, UART_VEC_NO, UART_IRQ_PRIO);
	uart_ready = 1;
	console_register(&g_console_ops);
}

void uart_putc(char c)
{
	unsigned char b;

	if (c == '\n')
		uart_putc('\r');

	b = (unsigned char)c;
	for (;;) {
		if (write(STDOUT_FILENO, &b, 1) == 1)
			return;
		if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
			return;
	}
}

void uart_puts(const char *s)
{
	while (*s)
		uart_putc(*s++);
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
