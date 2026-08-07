#include "shell.h"
#include <string.h>

static char hist[SHELL_HIST_MAX][SHELL_LINE_MAX];
static int hist_count;
static int hist_head; /* next write index */

static void hist_push(const char *line)
{
  int n;
  if (!line || !line[0]) return;
  n = (int)strlen(line);
  if (n >= SHELL_LINE_MAX) n = SHELL_LINE_MAX - 1;
  memcpy(hist[hist_head], line, (size_t)n);
  hist[hist_head][n] = '\0';
  hist_head = (hist_head + 1) % SHELL_HIST_MAX;
  if (hist_count < SHELL_HIST_MAX) hist_count++;
}

static const char *hist_get(int back)
{
  int idx;
  if (back < 1 || back > hist_count) return 0;
  idx = hist_head - back;
  if (idx < 0) idx += SHELL_HIST_MAX;
  return hist[idx];
}

static void redraw(const char *prompt, const char *buf, int len, int pos)
{
  int i;
  shell_putc('\r');
  shell_puts(prompt);
  for (i = 0; i < len; i++) shell_putc(buf[i]);
  shell_puts("\033[K");
  /* move cursor back if needed */
  for (i = len; i > pos; i--)
    shell_puts("\033[D");
}

int lineedit_read(char *buf, int buflen, const char *prompt)
{
  int len = 0, pos = 0, c, hist_nav = 0;
  char seq[2];

  if (!buf || buflen < 2) return -1;
  if (!prompt) prompt = "> ";
  buf[0] = '\0';
  shell_puts(prompt);

  for (;;) {
    c = shell_getc();
    if (c < 0) continue;

    if (c == '\r' || c == '\n') {
      shell_puts("\r\n");
      buf[len] = '\0';
      hist_push(buf);
      return len;
    }
    if (c == 0x7f || c == 0x08) { /* backspace */
      if (pos > 0) {
        int i;
        memmove(buf + pos - 1, buf + pos, (size_t)(len - pos));
        pos--;
        len--;
        buf[len] = '\0';
        redraw(prompt, buf, len, pos);
      }
      continue;
    }
    if (c == 0x03) { /* Ctrl-C */
      shell_puts("^C\r\n");
      buf[0] = '\0';
      return 0;
    }
    if (c == 0x1b) { /* escape / arrows */
      seq[0] = (char)shell_getc();
      seq[1] = (char)shell_getc();
      if (seq[0] == '[') {
        if (seq[1] == 'A') { /* up */
          const char *h;
          hist_nav++;
          h = hist_get(hist_nav);
          if (!h) { hist_nav--; continue; }
          len = (int)strlen(h);
          if (len >= buflen) len = buflen - 1;
          memcpy(buf, h, (size_t)len);
          buf[len] = '\0';
          pos = len;
          redraw(prompt, buf, len, pos);
        } else if (seq[1] == 'B') { /* down */
          const char *h;
          if (hist_nav <= 0) continue;
          hist_nav--;
          if (hist_nav == 0) {
            len = 0; pos = 0; buf[0] = '\0';
          } else {
            h = hist_get(hist_nav);
            if (!h) continue;
            len = (int)strlen(h);
            if (len >= buflen) len = buflen - 1;
            memcpy(buf, h, (size_t)len);
            buf[len] = '\0';
            pos = len;
          }
          redraw(prompt, buf, len, pos);
        } else if (seq[1] == 'C') { /* right */
          if (pos < len) { pos++; shell_puts("\033[C"); }
        } else if (seq[1] == 'D') { /* left */
          if (pos > 0) { pos--; shell_puts("\033[D"); }
        }
      }
      continue;
    }
    if (c < 0x20 || c > 0x7e) continue;
    if (len + 1 >= buflen) continue;
    if (pos < len)
      memmove(buf + pos + 1, buf + pos, (size_t)(len - pos));
    buf[pos] = (char)c;
    pos++;
    len++;
    buf[len] = '\0';
    hist_nav = 0;
    redraw(prompt, buf, len, pos);
  }
}
