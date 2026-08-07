#include "drv/console.h"

static const struct console_ops *g_con;

int console_register(const struct console_ops *ops)
{
  if (!ops || !ops->putc || !ops->getc)
    return -1;
  g_con = ops;
  return 0;
}

void console_init(void)
{
  if (g_con && g_con->init)
    g_con->init();
}

void console_putc(char c)
{
  if (g_con && g_con->putc)
    g_con->putc(c);
}

void console_puts(const char *s)
{
  if (!s)
    return;
  while (*s)
    console_putc(*s++);
}

int console_getc(void)
{
  if (g_con && g_con->getc)
    return g_con->getc();
  return -1;
}
