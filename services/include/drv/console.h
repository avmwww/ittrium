#ifndef ITT_DRV_CONSOLE_H
#define ITT_DRV_CONSOLE_H

struct console_ops {
  void (*init)(void);
  void (*putc)(char c);
  int (*getc)(void);
};

int console_register(const struct console_ops *ops);
void console_init(void);
void console_putc(char c);
void console_puts(const char *s);
int console_getc(void);

#endif
