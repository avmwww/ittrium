#include "shell.h"
#include <string.h>

enum chain_op {
  CHAIN_SEMI = 0,
  CHAIN_AND,
  CHAIN_OR
};

static int is_name_start(char c)
{
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

static int is_name_char(char c)
{
  return is_name_start(c) || (c >= '0' && c <= '9');
}

/* Strip # comments outside quotes (in place). */
static void strip_comment(char *line)
{
  char *p = line;
  char q = 0;
  while (*p) {
    if (q) {
      if (*p == q) q = 0;
      p++;
      continue;
    }
    if (*p == '\'' || *p == '"') {
      q = *p++;
      continue;
    }
    if (*p == '#') {
      *p = '\0';
      return;
    }
    p++;
  }
}

/*
 * Expand $VAR / ${VAR} into out. Single-quoted segments are copied raw
 * (caller tokenizes after — we expand on the whole segment first for
 * unquoted and double-quoted regions).
 *
 * Simpler approach: expand only outside single quotes.
 */
static int expand_vars(const char *in, char *out, int outsz)
{
  int o = 0;
  char q = 0;

  if (!in || !out || outsz < 2) return -1;
  while (*in && o + 1 < outsz) {
    if (q == '\'') {
      if (*in == '\'') q = 0;
      else out[o++] = *in;
      in++;
      continue;
    }
    if (!q && *in == '\'') {
      q = '\'';
      out[o++] = *in++;
      continue;
    }
    if (!q && *in == '"') {
      q = '"';
      out[o++] = *in++;
      continue;
    }
    if (q == '"' && *in == '"') {
      q = 0;
      out[o++] = *in++;
      continue;
    }
    if ((q == '"' || !q) && *in == '$') {
      const char *name;
      char nbuf[SHELL_VAR_NAME];
      int ni = 0;
      const char *val;
      in++;
      if (*in == '{') {
        in++;
        while (*in && *in != '}' && ni + 1 < (int)sizeof(nbuf))
          nbuf[ni++] = *in++;
        if (*in == '}') in++;
        nbuf[ni] = '\0';
        name = nbuf;
      } else if (is_name_start(*in)) {
        while (is_name_char(*in) && ni + 1 < (int)sizeof(nbuf))
          nbuf[ni++] = *in++;
        nbuf[ni] = '\0';
        name = nbuf;
      } else {
        out[o++] = '$';
        continue;
      }
      val = shell_var_get(name);
      if (val) {
        while (*val && o + 1 < outsz)
          out[o++] = *val++;
      }
      continue;
    }
    out[o++] = *in++;
  }
  out[o] = '\0';
  return o;
}

/* Tokenize expanded line into argv; mutates s. Returns argc. */
static int tokenize(char *s, char **argv, int max)
{
  int argc = 0;
  char *p = s;

  while (*p && argc < max) {
    char q;
    while (*p == ' ' || *p == '\t') p++;
    if (!*p) break;
    if (*p == '\'' || *p == '"') {
      q = *p++;
      argv[argc++] = p;
      while (*p && *p != q) p++;
      if (*p) *p++ = '\0';
      continue;
    }
    argv[argc++] = p;
    while (*p && *p != ' ' && *p != '\t' && *p != '\'' && *p != '"')
      p++;
    if (*p == '\'' || *p == '"') {
      /* adjacent quote: terminate token before quote */
      *p = '\0';
      continue;
    }
    if (*p) *p++ = '\0';
  }
  return argc;
}

static int is_assign(const char *tok, char *name, int nsz, const char **val)
{
  const char *eq;
  int i;
  if (!tok || !is_name_start(tok[0])) return 0;
  eq = strchr(tok, '=');
  if (!eq || eq == tok) return 0;
  for (i = 0; tok + i < eq; i++)
    if (!is_name_char(tok[i])) return 0;
  if (eq - tok >= nsz) return 0;
  memcpy(name, tok, (size_t)(eq - tok));
  name[eq - tok] = '\0';
  *val = eq + 1;
  return 1;
}

static int run_segment(char *seg)
{
  char expanded[SHELL_LINE_MAX];
  char *argv[SHELL_MAX_ARGS];
  char name[SHELL_VAR_NAME];
  const char *val;
  int argc, i, cmd0;

  while (*seg == ' ' || *seg == '\t') seg++;
  if (!*seg) return 0;

  if (expand_vars(seg, expanded, (int)sizeof(expanded)) < 0)
    return -1;

  argc = tokenize(expanded, argv, SHELL_MAX_ARGS);
  if (argc <= 0) return 0;

  cmd0 = 0;
  for (i = 0; i < argc; i++) {
    if (!is_assign(argv[i], name, (int)sizeof(name), &val))
      break;
    shell_var_set(name, val);
    cmd0 = i + 1;
  }
  if (cmd0 >= argc)
    return 0;

  return shell_run_argv(argc - cmd0, argv + cmd0);
}

/*
 * Split line into segments separated by ; && || (outside quotes).
 * Runs them according to chain rules. Mutates a working copy of line.
 */
int shell_exec_line(const char *line)
{
  char buf[SHELL_LINE_MAX];
  char *p, *start;
  char q = 0;
  int status = 0;
  enum chain_op next_op = CHAIN_SEMI;
  int run_next = 1;
  size_t n;

  if (!line) return 0;
  n = strlen(line);
  if (n >= sizeof(buf)) n = sizeof(buf) - 1;
  memcpy(buf, line, n);
  buf[n] = '\0';

  strip_comment(buf);

  p = buf;
  start = p;
  while (*p) {
    if (q) {
      if (*p == q) q = 0;
      p++;
      continue;
    }
    if (*p == '\'' || *p == '"') {
      q = *p++;
      continue;
    }
    if (*p == ';' ||
        (*p == '&' && p[1] == '&') ||
        (*p == '|' && p[1] == '|')) {
      enum chain_op this_sep;
      char *seg_end = p;

      if (*p == ';') {
        this_sep = CHAIN_SEMI;
        *seg_end = '\0';
        p++;
      } else if (*p == '&') {
        this_sep = CHAIN_AND;
        *seg_end = '\0';
        p += 2;
      } else {
        this_sep = CHAIN_OR;
        *seg_end = '\0';
        p += 2;
      }

      if (run_next) {
        while (*start == ' ' || *start == '\t') start++;
        if (*start)
          status = run_segment(start);
      }
      next_op = this_sep;
      if (next_op == CHAIN_SEMI)
        run_next = 1;
      else if (next_op == CHAIN_AND)
        run_next = (status == 0);
      else
        run_next = (status != 0);
      start = p;
      continue;
    }
    p++;
  }

  if (run_next) {
    while (*start == ' ' || *start == '\t') start++;
    if (*start)
      status = run_segment(start);
  }
  return status;
}
