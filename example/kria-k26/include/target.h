#ifndef _TARGET_H_
#define _TARGET_H_

#include <stdint.h>

struct netdev;

/* Kria K26 SOM (Zynq UltraScale+ MPSoC) — PS phys addresses */
#define GICD_BASE       0xF9010000UL
#define GICC_BASE       0xF9020000UL
#define GIC_USE_GROUP1  1

#define UART_BASE       0xFF010000UL  /* UART1 Cadence */
#define UART_IRQ        54u
#define UART_IRQ_PRIO   0xa0u
#define UART_CLK_HZ     100000000u
#define UART_BAUD       115200u

#define GEM_BASE        0xFF0E0000UL  /* GEM3 */
#define GEM_IRQ         95u
#define GEM_IRQ_PRIO    0xb0u
#define GEM_PHY_ADDR    0u            /* Marvell 88E1512 @ MDIO 0 */

#define TIMER_IRQ       30u           /* CNTP NS PPI */

/* IOU system counter (enable if firmware left it off) */
#define IOU_SCNTRS_BASE 0xFF260000UL

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
int uart_getc(void);

int gem_net_init(void);
struct netdev *gem_net_dev(void);

#endif /* _TARGET_H_ */
