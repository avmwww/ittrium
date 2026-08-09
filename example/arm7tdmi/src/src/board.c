#include "ittrium.h"

/* Soft stubs so the port links without a full LPC VIC BSP. */
int install_IRQ(int vec, int prio, void (*handler)(void))
{
  (void)vec; (void)prio; (void)handler;
  return 1; /* TRUE */
}

void init_interrupt(void)
{
}
