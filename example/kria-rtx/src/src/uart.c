/**
 * Cadence UART (ZynqMP UART1): IRQ + eventflag + soft RX/TX FIFO
 */
#include "ittrium.h"
#include "target.h"
#include "drv/console.h"

#define UART_CR       (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_MR       (*(volatile uint32_t *)(UART_BASE + 0x04))
#define UART_IER      (*(volatile uint32_t *)(UART_BASE + 0x08))
#define UART_IDR      (*(volatile uint32_t *)(UART_BASE + 0x0c))
#define UART_IMR      (*(volatile uint32_t *)(UART_BASE + 0x10))
#define UART_ISR      (*(volatile uint32_t *)(UART_BASE + 0x14))
#define UART_BAUDGEN  (*(volatile uint32_t *)(UART_BASE + 0x18))
#define UART_RXTOUT   (*(volatile uint32_t *)(UART_BASE + 0x1c))
#define UART_RXWM     (*(volatile uint32_t *)(UART_BASE + 0x20))
#define UART_SR       (*(volatile uint32_t *)(UART_BASE + 0x2c))
#define UART_FIFO     (*(volatile uint32_t *)(UART_BASE + 0x30))
#define UART_BAUDDIV  (*(volatile uint32_t *)(UART_BASE + 0x34))

#define CR_RXRST      (1u << 0)
#define CR_TXRST      (1u << 1)
#define CR_RX_EN      (1u << 2)
#define CR_RX_DIS     (1u << 3)
#define CR_TX_EN      (1u << 4)
#define CR_TX_DIS     (1u << 5)

#define MR_CHMODE_NORM 0u
#define MR_PARITY_NONE (0x4u << 3)
#define MR_CHARLEN_8   (0u << 1)
#define MR_STOP_1      (0u << 6)

#define SR_RXEMPTY    (1u << 1)
#define SR_TXFULL     (1u << 4)
#define SR_TXEMPTY    (1u << 3)

#define IXR_RXOVR     (1u << 0)
#define IXR_RXFULL    (1u << 1)
#define IXR_TXEMPTY   (1u << 3)
#define IXR_RXEMPTY   (1u << 5)
#define IXR_TXFULL    (1u << 4)
#define IXR_TOUT      (1u << 8)
#define IXR_RX        (IXR_RXOVR | IXR_RXFULL | IXR_TOUT)

#define UART_FIFO_SIZE 128
#define UART_FLG_RX    (1u << 0)
#define UART_FLG_TX    (1u << 1)

static uint8_t rx_fifo[UART_FIFO_SIZE];
static uint8_t tx_fifo[UART_FIFO_SIZE];
static volatile unsigned rx_head, rx_tail, rx_count;
static volatile unsigned tx_head, tx_tail, tx_count;
static int uart_ready;

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

static void uart_baud(uint32_t clk, uint32_t baud)
{
  uint32_t brgr, bauddiv;
  /* Iterative CD/BDIV like XUartPs_SetBaudRate (8N1) */
  for (bauddiv = 4; bauddiv < 255; bauddiv++) {
    brgr = clk / (baud * (bauddiv + 1u));
    if (brgr > 0 && brgr < 65536u) {
      UART_BAUDGEN = brgr;
      UART_BAUDDIV = bauddiv;
      return;
    }
  }
  UART_BAUDGEN = clk / (baud * 16u);
  UART_BAUDDIV = 15;
}

static void uart_tx_kick(void)
{
  uint8_t b;

  while (!(UART_SR & SR_TXFULL) && tx_pop(&b))
    UART_FIFO = b;

  if (tx_count)
    UART_IER = IXR_TXEMPTY;
  else
    UART_IDR = IXR_TXEMPTY;
}

static void uart_irq(void)
{
  uint32_t isr = UART_ISR;
  FLGPTN set = 0;

  if (isr & IXR_RX) {
    while (!(UART_SR & SR_RXEMPTY))
      rx_push((uint8_t)(UART_FIFO & 0xffu));
    UART_ISR = IXR_RX;
    if (rx_count)
      set |= UART_FLG_RX;
  }

  if (isr & IXR_TXEMPTY) {
    UART_ISR = IXR_TXEMPTY;
    uart_tx_kick();
    if (tx_count < UART_FIFO_SIZE)
      set |= UART_FLG_TX;
  }

  if (set)
    iset_flg(UART_FLG_ID, set);
}

static const struct console_ops g_console_ops = {
  .putc = uart_putc,
  .getc = uart_getc,
};

void uart_init(void)
{
  T_CFLG pk_cflg;

  rx_head = rx_tail = rx_count = 0;
  tx_head = tx_tail = tx_count = 0;

  pk_cflg.flgatr = TA_TFIFO | TA_CLR | TA_WMUL;
  pk_cflg.iflgptn = UART_FLG_TX;
  cre_flg(UART_FLG_ID, &pk_cflg);

  UART_IDR = 0x1fffu;
  UART_CR = CR_RX_DIS | CR_TX_DIS | CR_TXRST | CR_RXRST;
  uart_baud(UART_CLK_HZ, UART_BAUD);
  UART_MR = MR_CHMODE_NORM | MR_PARITY_NONE | MR_CHARLEN_8 | MR_STOP_1;
  UART_RXWM = 8;
  UART_RXTOUT = 1;
  UART_ISR = 0x1fffu;
  UART_CR = CR_RX_EN | CR_TX_EN;
  UART_IER = IXR_RX;

  install_handler(uart_irq, UART_VEC_NO, UART_IRQ_PRIO);
  gic_enable_irq(UART_IRQ, UART_IRQ_PRIO);
  uart_ready = 1;
  console_register(&g_console_ops);
}

void uart_putc(char c)
{
  lock_state_t ls;
  FLGPTN ptn;

  if (c == '\n')
    uart_putc('\r');

  if (!uart_ready) {
    while (UART_SR & SR_TXFULL)
      ;
    UART_FIFO = (uint32_t)(uint8_t)c;
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

int __io_putchar(int ch)
{
  uart_putc((char)ch);
  return ch;
}

static void uart_putc_raw(char c)
{
  while (UART_SR & SR_TXFULL)
    ;
  UART_FIFO = (uint32_t)(uint8_t)c;
}

static void uart_puts_raw(const char *s)
{
  while (*s)
    uart_putc_raw(*s++);
}

static void uart_put_hex64(uint64_t v)
{
  static const char hex[] = "0123456789abcdef";
  int i;
  for (i = 60; i >= 0; i -= 4)
    uart_putc_raw(hex[(v >> i) & 0xfu]);
}

void sync_abort_dump(uint64_t esr, uint64_t far, uint64_t elr)
{
  uart_puts_raw("\r\nSYNC ABORT esr=");
  uart_put_hex64(esr);
  uart_puts_raw(" far=");
  uart_put_hex64(far);
  uart_puts_raw(" elr=");
  uart_put_hex64(elr);
  uart_puts_raw("\r\n");
}
