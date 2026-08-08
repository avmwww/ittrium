#include "shell.h"
#include <string.h>

struct shell_var {
  char name[SHELL_VAR_NAME];
  char val[SHELL_VAR_VAL];
  int used;
};

static struct shell_var g_vars[SHELL_VAR_MAX];

static int valid_name_start(char c)
{
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

static int valid_name_char(char c)
{
  return valid_name_start(c) || (c >= '0' && c <= '9');
}

int shell_var_set(const char *name, const char *val)
{
  int i, free_i = -1;
  size_t nlen, vlen;

  if (!name || !valid_name_start(name[0])) return -1;
  for (nlen = 0; name[nlen]; nlen++)
    if (!valid_name_char(name[nlen])) return -1;
  if (nlen == 0 || nlen >= SHELL_VAR_NAME) return -1;
  if (!val) val = "";
  vlen = strlen(val);
  if (vlen >= SHELL_VAR_VAL) vlen = SHELL_VAR_VAL - 1;

  for (i = 0; i < SHELL_VAR_MAX; i++) {
    if (!g_vars[i].used) {
      if (free_i < 0) free_i = i;
      continue;
    }
    if (strcmp(g_vars[i].name, name) == 0) {
      memcpy(g_vars[i].val, val, vlen);
      g_vars[i].val[vlen] = '\0';
      return 0;
    }
  }
  if (free_i < 0) return -1;
  memcpy(g_vars[free_i].name, name, nlen);
  g_vars[free_i].name[nlen] = '\0';
  memcpy(g_vars[free_i].val, val, vlen);
  g_vars[free_i].val[vlen] = '\0';
  g_vars[free_i].used = 1;
  return 0;
}

int shell_var_unset(const char *name)
{
  int i;
  if (!name) return -1;
  for (i = 0; i < SHELL_VAR_MAX; i++) {
    if (g_vars[i].used && strcmp(g_vars[i].name, name) == 0) {
      g_vars[i].used = 0;
      g_vars[i].name[0] = '\0';
      g_vars[i].val[0] = '\0';
      return 0;
    }
  }
  return -1;
}

const char *shell_var_get(const char *name)
{
  int i;
  if (!name) return 0;
  for (i = 0; i < SHELL_VAR_MAX; i++)
    if (g_vars[i].used && strcmp(g_vars[i].name, name) == 0)
      return g_vars[i].val;
  return 0;
}

void shell_var_print_all(void)
{
  int i;
  for (i = 0; i < SHELL_VAR_MAX; i++) {
    if (!g_vars[i].used) continue;
    shell_puts(g_vars[i].name);
    shell_putc('=');
    shell_puts(g_vars[i].val);
    shell_puts("\r\n");
  }
}
