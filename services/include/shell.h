#ifndef ITT_SHELL_H
#define ITT_SHELL_H

#include "ittrium.h"

#define SHELL_MAX_CMDS   32
#define SHELL_LINE_MAX   128
#define SHELL_HIST_MAX   8
#define SHELL_MAX_ARGS   16

typedef int (*shell_cmd_fn)(int argc, char **argv);

typedef void (*shell_putc_fn)(char c);
typedef int (*shell_getc_fn)(void); /* blocking; return char or -1 */

void shell_set_io(shell_putc_fn putc_fn, shell_getc_fn getc_fn);
void shell_putc(char c);
void shell_puts(const char *s);
int shell_getc(void);

int shell_register(const char *name, const char *help, shell_cmd_fn fn);
void shell_init_builtins(void);
void shell_task(void *exinf);

/* line editor (linenoise-like) */
int lineedit_read(char *buf, int buflen, const char *prompt);

#endif
