#include "shell.h"
#include "vfs.h"
#include "elf_load.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static shell_putc_fn g_putc;
static shell_getc_fn g_getc;

struct shell_cmd {
  const char *name;
  const char *help;
  shell_cmd_fn fn;
};

static struct shell_cmd cmds[SHELL_MAX_CMDS];
static int ncmds;

void shell_set_io(shell_putc_fn putc_fn, shell_getc_fn getc_fn)
{
  g_putc = putc_fn;
  g_getc = getc_fn;
}

void shell_putc(char c)
{
  if (g_putc) g_putc(c);
}

void shell_puts(const char *s)
{
  if (!s) return;
  while (*s) shell_putc(*s++);
}

int shell_getc(void)
{
  if (!g_getc) return -1;
  return g_getc();
}

static void shell_printf(const char *fmt, ...)
{
  /* Minimal formatter: supports %s %u %d %% only (no libc snprintf — nano can hang). */
  va_list ap;
  const char *p;

  va_start(ap, fmt);
  for (p = fmt; *p; p++) {
    if (*p != '%') {
      shell_putc(*p);
      continue;
    }
    p++;
    if (*p == '%') {
      shell_putc('%');
    } else if (*p == 's') {
      const char *s = va_arg(ap, const char *);
      if (s) shell_puts(s);
    } else if (*p == 'u' || *p == 'd') {
      unsigned v;
      char tmp[16];
      int i = 0;
      if (*p == 'd') {
        int d = va_arg(ap, int);
        if (d < 0) {
          shell_putc('-');
          v = (unsigned)(-d);
        } else {
          v = (unsigned)d;
        }
      } else {
        v = va_arg(ap, unsigned);
      }
      if (v == 0) {
        shell_putc('0');
      } else {
        while (v) {
          tmp[i++] = (char)('0' + (v % 10u));
          v /= 10u;
        }
        while (i--)
          shell_putc(tmp[i]);
      }
    } else if (*p) {
      shell_putc(*p);
    }
  }
  va_end(ap);
}

int shell_register(const char *name, const char *help, shell_cmd_fn fn)
{
  int i;
  if (!name || !fn || ncmds >= SHELL_MAX_CMDS) return -1;
  for (i = 0; i < ncmds; i++) {
    if (strcmp(cmds[i].name, name) == 0) {
      cmds[i].help = help;
      cmds[i].fn = fn;
      return 0;
    }
  }
  cmds[ncmds].name = name;
  cmds[ncmds].help = help ? help : "";
  cmds[ncmds].fn = fn;
  ncmds++;
  return 0;
}

static int cmd_help(int argc, char **argv)
{
  int i;
  (void)argc; (void)argv;
  shell_puts("commands:\r\n");
  for (i = 0; i < ncmds; i++) {
    shell_puts("  ");
    shell_puts(cmds[i].name);
    shell_puts(" - ");
    shell_puts(cmds[i].help ? cmds[i].help : "");
    shell_puts("\r\n");
  }
  return 0;
}

static int cmd_ps(int argc, char **argv)
{
  int fd, n;
  char buf[128];
  (void)argc; (void)argv;
  fd = vfs_open("/proc/tasks", VFS_O_RDONLY);
  if (fd < 0) {
    shell_puts("ps: /proc/tasks unavailable\r\n");
    return -1;
  }
  while ((n = vfs_read(fd, buf, sizeof(buf) - 1)) > 0) {
    buf[n] = '\0';
    shell_puts(buf);
  }
  vfs_close(fd);
  return 0;
}

static int cmd_ticks(int argc, char **argv)
{
  int fd, n;
  char buf[64];
  (void)argc; (void)argv;
  fd = vfs_open("/proc/ticks", VFS_O_RDONLY);
  if (fd < 0) {
    SYSTIM t;
    if (get_tim(&t) == E_OK)
      shell_printf("%u:%u\r\n", (unsigned)t.st_high, (unsigned)t.st_low);
    else
      shell_puts("ticks: n/a\r\n");
    return 0;
  }
  while ((n = vfs_read(fd, buf, sizeof(buf) - 1)) > 0) {
    buf[n] = '\0';
    shell_puts(buf);
  }
  vfs_close(fd);
  return 0;
}

static int cmd_ls(int argc, char **argv)
{
  const char *path = "/";
  char name[VFS_NAME_MAX];
  int idx = 0, rc;
  struct vfs_stat st;

  if (argc >= 2) path = argv[1];
  if (vfs_stat(path, &st) == 0 && st.type == VFS_TYPE_REG) {
    shell_printf("%s  %u\r\n", path, (unsigned)st.size);
    return 0;
  }
  while ((rc = vfs_listdir(path, name, sizeof(name), &idx)) == 0) {
    shell_puts(name);
    shell_puts("\r\n");
  }
  if (rc < 0) shell_puts("ls: error\r\n");
  return 0;
}

static int cmd_cat(int argc, char **argv)
{
  int fd, n;
  char buf[64];
  if (argc < 2) {
    shell_puts("usage: cat <path>\r\n");
    return -1;
  }
  fd = vfs_open(argv[1], VFS_O_RDONLY);
  if (fd < 0) {
    shell_puts("cat: open failed\r\n");
    return -1;
  }
  while ((n = vfs_read(fd, buf, sizeof(buf) - 1)) > 0) {
    int i;
    for (i = 0; i < n; i++) shell_putc(buf[i]);
  }
  vfs_close(fd);
  shell_puts("\r\n");
  return 0;
}

static int cmd_mount(int argc, char **argv)
{
  int i, n;
  (void)argc; (void)argv;
  n = vfs_mount_count();
  for (i = 0; i < n; i++) {
    const char *p = vfs_mount_path(i);
    if (p) shell_printf("%s\r\n", p);
  }
  return 0;
}

/* Weak stub if elf_load not linked */
__attribute__((weak)) ER elf_run(const char *path, ID tskid, VP stack, SIZE stksz, PRI pri)
{
  (void)path; (void)tskid; (void)stack; (void)stksz; (void)pri;
  return E_NOSPT;
}

static int cmd_run(int argc, char **argv)
{
  ER er;
  ID tid = 10;
  PRI pri = 5;
  static uint8_t stack[4096];

  if (argc < 2) {
    shell_puts("usage: run <path> [tskid] [pri]\r\n");
    return -1;
  }
  if (argc >= 3) tid = (ID)atoi(argv[2]);
  if (argc >= 4) pri = (PRI)atoi(argv[3]);
  er = elf_run(argv[1], tid, stack, (SIZE)sizeof(stack), pri);
  if (er != E_OK) {
    shell_printf("run: failed (%d)\r\n", (int)er);
    return -1;
  }
  shell_printf("run: task %d started\r\n", (int)tid);
  return 0;
}

static int cmd_irq(int argc, char **argv)
{
  int fd, n;
  char buf[128];
  (void)argc;
  (void)argv;
  fd = vfs_open("/proc/interrupts", VFS_O_RDONLY);
  if (fd < 0) {
    shell_puts("irq: /proc/interrupts unavailable\r\n");
    return -1;
  }
  while ((n = vfs_read(fd, buf, sizeof(buf) - 1)) > 0) {
    buf[n] = '\0';
    shell_puts(buf);
  }
  vfs_close(fd);
  return 0;
}

static int cmd_load(int argc, char **argv)
{
  int fd, n;
  char buf[80];
  (void)argc;
  (void)argv;
  fd = vfs_open("/sys/cpu/load", VFS_O_RDONLY);
  if (fd < 0) {
    fd = vfs_open("/proc/stat", VFS_O_RDONLY);
    if (fd < 0) {
      shell_puts("load: n/a\r\n");
      return -1;
    }
  }
  while ((n = vfs_read(fd, buf, sizeof(buf) - 1)) > 0) {
    buf[n] = '\0';
    shell_puts(buf);
  }
  vfs_close(fd);
  return 0;
}

void shell_init_builtins(void)
{
  shell_register("help",  "list commands", cmd_help);
  shell_register("ps",    "list tasks (/proc/tasks)", cmd_ps);
  shell_register("ticks", "system ticks", cmd_ticks);
  shell_register("irq",   "IRQ counters (/proc/interrupts)", cmd_irq);
  shell_register("load",  "CPU idle% (/sys/cpu/load)", cmd_load);
  shell_register("ls",    "list directory", cmd_ls);
  shell_register("cat",   "print file", cmd_cat);
  shell_register("mount", "list mounts", cmd_mount);
  shell_register("run",   "load and run ELF/ITRM", cmd_run);
}

static int split_args(char *line, char **argv, int max)
{
  int argc = 0;
  char *p = line;
  while (*p && argc < max) {
    while (*p == ' ' || *p == '\t') p++;
    if (!*p) break;
    argv[argc++] = p;
    while (*p && *p != ' ' && *p != '\t') p++;
    if (*p) *p++ = '\0';
  }
  return argc;
}

static int shell_dispatch(int argc, char **argv)
{
  int i;
  if (argc <= 0) return 0;
  for (i = 0; i < ncmds; i++) {
    if (strcmp(cmds[i].name, argv[0]) == 0)
      return cmds[i].fn(argc, argv);
  }
  shell_printf("unknown: %s\r\n", argv[0]);
  return -1;
}

void shell_task(void *exinf)
{
  char line[SHELL_LINE_MAX];
  char *argv[SHELL_MAX_ARGS];
  int argc;

  (void)exinf;
  shell_init_builtins();
  shell_puts("ittrium shell\r\n");

  for (;;) {
    if (lineedit_read(line, sizeof(line), "ittrium> ") < 0)
      continue;
    argc = split_args(line, argv, SHELL_MAX_ARGS);
    if (argc > 0)
      shell_dispatch(argc, argv);
  }
}
