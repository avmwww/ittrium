#ifndef ITT_SHELL_H
#define ITT_SHELL_H

#include "ittrium.h"

#define SHELL_MAX_CMDS   32
#define SHELL_LINE_MAX   192
#define SHELL_HIST_MAX   16
#define SHELL_MAX_ARGS   16
#define SHELL_VAR_MAX    16
#define SHELL_VAR_NAME   24
#define SHELL_VAR_VAL    64

typedef int (*shell_cmd_fn)(int argc, char **argv);

typedef void (*shell_putc_fn)(char c);
typedef int (*shell_getc_fn)(void); /* blocking; return char or -1 */

void shell_set_io(shell_putc_fn putc_fn, shell_getc_fn getc_fn);
void shell_putc(char c);
void shell_puts(const char *s);
int shell_getc(void);
void shell_printf(const char *fmt, ...);

int shell_register(const char *name, const char *help, shell_cmd_fn fn);
void shell_init_builtins(void);
void shell_net_register(void); /* weak stub if no lwIP */
void shell_task(void *exinf);

/* Parse/expand/run one line (mutates a local copy). Returns last status. */
int shell_exec_line(const char *line);

/* Run argv[0] as builtin; return status (0 ok, non-zero fail). */
int shell_run_argv(int argc, char **argv);

/* Tab-complete command name into buf[0..*len). Returns match count. */
int shell_complete_cmds(char *buf, int buflen, int *len, int *pos, int list);

/* Session variables */
int shell_var_set(const char *name, const char *val);
int shell_var_unset(const char *name);
const char *shell_var_get(const char *name);
void shell_var_print_all(void);

/* line editor (linenoise-like) */
int lineedit_read(char *buf, int buflen, const char *prompt);
int lineedit_hist_count(void);
const char *lineedit_hist_entry(int back); /* 1 = most recent */

#endif
