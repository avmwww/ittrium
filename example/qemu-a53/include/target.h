#ifndef _TARGET_H_
#define _TARGET_H_

#include <stdint.h>

struct netdev;

/* QEMU virt machine (gic-version=2) */
#define GICD_BASE       0x08000000UL
#define GICC_BASE       0x08010000UL
#define UART_BASE       0x09000000UL
#define TIMER_IRQ       30u
#define UART_IRQ        33u
#define UART_IRQ_PRIO   0xa0u
/* virtio-mmio slot0: DTB SPI 16 → GIC 48 */
#define VIRTIO_MMIO_BASE 0x0a000000UL
#define VIRTIO_NET_IRQ   48u
#define VIRTIO_NET_IRQ_PRIO 0xb0u

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
int uart_getc(void);

int virtio_net_init(void);
struct netdev *virtio_net_dev(void);

#endif /* _TARGET_H_ */
