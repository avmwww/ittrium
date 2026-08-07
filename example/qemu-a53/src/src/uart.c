/**
 * PL011 UART: IRQ + eventflag + soft RX/TX FIFO
 */
#include "ittrium.h"
#include "target.h"
#include "drv/console.h"

#define UART_DR       (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_FR       (*(volatile uint32_t *)(UART_BASE + 0x18))
#define UART_LCR_H    (*(volatile uint32_t *)(UART_BASE + 0x2c))
#define UART_CR       (*(volatile uint32_t *)(UART_BASE + 0x30))
#define UART_IMSC     (*(volatile uint32_t *)(UART_BASE + 0x38))
#define UART_MIS      (*(volatile uint32_t *)(UART_BASE + 0x40))
#define UART_ICR      (*(volatile uint32_t *)(UART_BASE + 0x44))

#define UART_FR_RXFE  (1u << 4)
#define UART_FR_TXFF  (1u << 5)

#define UART_CR_UARTEN (1u << 0)
#define UART_CR_TXE    (1u << 8)
#define UART_CR_RXE    (1u << 9)

#define UART_LCR_H_FEN (1u << 4)
#define UART_LCR_H_WLEN_8 (3u << 5)

#define UART_INT_RX   (1u << 4)
#define UART_INT_TX   (1u << 5)
#define UART_INT_RT   (1u << 6)

#define UART_FIFO_SIZE 128

#define UART_FLG_RX   (1u << 0)
#define UART_FLG_TX   (1u << 1)

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

/* Soft TX → PL011; TX IRQ only while soft TX non-empty */
static void uart_tx_kick(void)
{
  uint8_t b;

  while (!(UART_FR & UART_FR_TXFF) && tx_pop(&b))
    UART_DR = b;

  if (tx_count)
    UART_IMSC |= UART_INT_TX;
  else
    UART_IMSC &= ~UART_INT_TX;
}

static void uart_irq(void)
{
  uint32_t mis = UART_MIS;
  FLGPTN set = 0;

  if (mis & (UART_INT_RX | UART_INT_RT)) {
    while (!(UART_FR & UART_FR_RXFE))
      rx_push((uint8_t)(UART_DR & 0xffu));
    UART_ICR = UART_INT_RX | UART_INT_RT;
    if (rx_count)
      set |= UART_FLG_RX;
  }

  if (mis & UART_INT_TX) {
    UART_ICR = UART_INT_TX;
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

  UART_CR = 0;
  UART_LCR_H = UART_LCR_H_FEN | UART_LCR_H_WLEN_8;
  UART_ICR = 0x7ff;
  UART_IMSC = UART_INT_RX | UART_INT_RT;
  UART_CR = UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE;

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
    while (UART_FR & UART_FR_TXFF)
      ;
    UART_DR = (uint32_t)(uint8_t)c;
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

/* Polled TX for sync_abort_dump (IRQs may be off) */
static void uart_putc_raw(char c)
{
  while (UART_FR & UART_FR_TXFF)
    ;
  UART_DR = (uint32_t)(uint8_t)c;
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
