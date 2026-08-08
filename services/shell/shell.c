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

void shell_printf(const char *fmt, ...)
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

/* Weak stubs if elf_load not linked */
__attribute__((weak)) ER_ID elf_run(const char *path, ID tskid, VP stack, SIZE stksz, PRI pri)
{
  (void)path; (void)tskid; (void)stack; (void)stksz; (void)pri;
  return E_NOSPT;
}

__attribute__((weak)) ER elf_kill(ID tskid)
{
  (void)tskid;
  return E_NOSPT;
}

static int cmd_run(int argc, char **argv)
{
  ER_ID id;
#if CFG_USE_ELF
  ID tid = (ID)ELF_TASK_ID; /* 0 => acre_tsk */
#else
  ID tid = TMIN_TSKID;
#endif
  PRI pri = 10;
#if defined(ELF_TSK_STACK_SIZE)
  SIZE stksz = (SIZE)ELF_TSK_STACK_SIZE;
#else
  SIZE stksz = 4096;
#endif

  if (argc < 2) {
    shell_puts("usage: run <path> [tskid] [pri]\r\n");
    return -1;
  }
  if (argc >= 3) tid = (ID)atoi(argv[2]);
  if (argc >= 4) pri = (PRI)atoi(argv[3]);
  id = elf_run(argv[1], tid, (VP)0, stksz, pri);
  if (id < 0) {
    shell_printf("run: failed (%d)\r\n", (int)id);
    return -1;
  }
  shell_printf("run: task %d started\r\n", (int)id);
  return 0;
}

static int cmd_kill(int argc, char **argv)
{
  ER er;
  ID tid;

  if (argc < 2) {
    shell_puts("usage: kill <tskid>\r\n");
    return -1;
  }
  tid = (ID)atoi(argv[1]);
  er = elf_kill(tid);
  if (er != E_OK) {
    shell_printf("kill: failed (%d)\r\n", (int)er);
    return -1;
  }
  shell_printf("kill: task %d unloaded\r\n", (int)tid);
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

static int cmd_echo(int argc, char **argv)
{
  int i;
  for (i = 1; i < argc; i++) {
    if (i > 1) shell_putc(' ');
    shell_puts(argv[i]);
  }
  shell_puts("\r\n");
  return 0;
}

static int cmd_set(int argc, char **argv)
{
  char name[SHELL_VAR_NAME];
  const char *eq, *val;
  int i;

  if (argc < 2) {
    shell_var_print_all();
    return 0;
  }
  for (i = 1; i < argc; i++) {
    eq = strchr(argv[i], '=');
    if (!eq || eq == argv[i]) {
      shell_puts("usage: set [NAME=value ...]\r\n");
      return -1;
    }
    if ((eq - argv[i]) >= (int)sizeof(name)) {
      shell_puts("set: name too long\r\n");
      return -1;
    }
    memcpy(name, argv[i], (size_t)(eq - argv[i]));
    name[eq - argv[i]] = '\0';
    val = eq + 1;
    if (shell_var_set(name, val) != 0) {
      shell_puts("set: failed\r\n");
      return -1;
    }
  }
  return 0;
}

static int cmd_unset(int argc, char **argv)
{
  int i;
  if (argc < 2) {
    shell_puts("usage: unset <NAME>...\r\n");
    return -1;
  }
  for (i = 1; i < argc; i++) {
    if (shell_var_unset(argv[i]) != 0) {
      shell_printf("unset: %s not set\r\n", argv[i]);
      return -1;
    }
  }
  return 0;
}

static int cmd_source(int argc, char **argv)
{
  int fd, n, i, len = 0;
  char chunk[64];
  char line[SHELL_LINE_MAX];
  int status = 0;

  if (argc < 2) {
    shell_puts("usage: source <path>\r\n");
    return -1;
  }
  fd = vfs_open(argv[1], VFS_O_RDONLY);
  if (fd < 0) {
    shell_puts("source: open failed\r\n");
    return -1;
  }
  while ((n = vfs_read(fd, chunk, sizeof(chunk))) > 0) {
    for (i = 0; i < n; i++) {
      char c = chunk[i];
      if (c == '\r') continue;
      if (c == '\n') {
        line[len] = '\0';
        if (len > 0)
          status = shell_exec_line(line);
        len = 0;
        continue;
      }
      if (len + 1 < (int)sizeof(line))
        line[len++] = c;
    }
  }
  if (len > 0) {
    line[len] = '\0';
    status = shell_exec_line(line);
  }
  vfs_close(fd);
  return status;
}

static int cmd_history(int argc, char **argv)
{
  int i, n;
  (void)argc;
  (void)argv;
  n = lineedit_hist_count();
  for (i = n; i >= 1; i--) {
    const char *h = lineedit_hist_entry(i);
    if (!h) continue;
    shell_printf("%d %s\r\n", n - i + 1, h);
  }
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
  shell_register("kill",  "unload ELF task by tskid", cmd_kill);
  shell_register("echo",  "print arguments", cmd_echo);
  shell_register("set",   "set/list variables", cmd_set);
  shell_register("unset", "unset variables", cmd_unset);
  shell_register("source","run commands from file", cmd_source);
  shell_register(".",     "run commands from file", cmd_source);
  shell_register("history","show command history", cmd_history);
  shell_net_register();
}

__attribute__((weak)) void shell_net_register(void)
{
}

int shell_run_argv(int argc, char **argv)
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

int shell_complete_cmds(char *buf, int buflen, int *len, int *pos, int list)
{
  char prefix[SHELL_LINE_MAX];
  int plen = 0, i, matches = 0;
  const char *first = 0;
  int common_len = -1;

  if (!buf || !len || !pos || *len < 0 || *len >= buflen) return 0;
  /* only complete first word */
  while (plen < *len && buf[plen] != ' ' && buf[plen] != '\t')
    plen++;
  if (plen < *len) return 0; /* already has args */
  if (plen >= (int)sizeof(prefix)) plen = (int)sizeof(prefix) - 1;
  memcpy(prefix, buf, (size_t)plen);
  prefix[plen] = '\0';

  for (i = 0; i < ncmds; i++) {
    if (strncmp(cmds[i].name, prefix, (size_t)plen) != 0)
      continue;
    matches++;
    if (!first) {
      first = cmds[i].name;
      common_len = (int)strlen(first);
    } else {
      int j = 0;
      while (j < common_len && first[j] && cmds[i].name[j] &&
             first[j] == cmds[i].name[j])
        j++;
      common_len = j;
    }
  }

  if (matches == 0) return 0;

  if (matches == 1 && first) {
    int n = (int)strlen(first);
    if (n >= buflen) n = buflen - 1;
    memcpy(buf, first, (size_t)n);
    buf[n] = '\0';
    *len = n;
    *pos = n;
    return 1;
  }

  if (common_len > plen && first) {
    if (common_len >= buflen) common_len = buflen - 1;
    memcpy(buf, first, (size_t)common_len);
    buf[common_len] = '\0';
    *len = common_len;
    *pos = common_len;
  }

  if (list) {
    shell_puts("\r\n");
    for (i = 0; i < ncmds; i++) {
      if (strncmp(cmds[i].name, prefix, (size_t)plen) != 0)
        continue;
      shell_puts(cmds[i].name);
      shell_puts("\r\n");
    }
  }
  return matches;
}

void shell_task(void *exinf)
{
  char line[SHELL_LINE_MAX];

  (void)exinf;
  shell_init_builtins();
  shell_puts("ittrium shell\r\n");

  for (;;) {
    if (lineedit_read(line, sizeof(line), "ittrium> ") < 0)
      continue;
    if (line[0])
      shell_exec_line(line);
  }
}
