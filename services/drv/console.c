#include "drv/console.h"
#include "drv/device.h"

static const struct console_ops *console_ops(void)
{
  struct itt_device *d = itt_device_find_class(ITT_DEV_CONSOLE, 0);

  return d ? (const struct console_ops *)d->drv : 0;
}

int console_register(const struct console_ops *ops)
{
  if (!ops || !ops->putc || !ops->getc)
    return -1;
  return itt_device_add("console", ITT_DEV_CONSOLE, (void *)ops);
}

void console_init(void)
{
  const struct console_ops *ops = console_ops();

  if (ops && ops->init)
    ops->init();
}

void console_putc(char c)
{
  const struct console_ops *ops = console_ops();

  if (ops && ops->putc)
    ops->putc(c);
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
  const struct console_ops *ops = console_ops();

  if (ops && ops->getc)
    return ops->getc();
  return -1;
}
