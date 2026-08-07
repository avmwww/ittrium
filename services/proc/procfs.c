#include "procfs.h"
#include "telemetry.h"
#include "task.h"
#include <stdlib.h>
#include <string.h>

#define PROC_BUF_MAX 1536

struct proc_file {
  char *buf;
  size_t len;
  size_t pos;
};

static const char *const proc_names[] = {
  "tasks", "ticks", "mem", "stat", "interrupts", "version", 0
};

extern char end;
extern void *_sbrk(int incr);

static void append_str(char *buf, size_t cap, int *n, const char *s)
{
  while (*s && (size_t)(*n) + 1 < cap)
    buf[(*n)++] = *s++;
}

static void append_uint(char *buf, size_t cap, int *n, unsigned v)
{
  char tmp[16];
  int i = 0;
  if (v == 0) {
    append_str(buf, cap, n, "0");
    return;
  }
  while (v && i < (int)sizeof(tmp)) {
    tmp[i++] = (char)('0' + (v % 10u));
    v /= 10u;
  }
  while (i-- && (size_t)(*n) + 1 < cap)
    buf[(*n)++] = tmp[i];
}

/* Left- or right-aligned field of exact width (truncated / pad with spaces). */
static void append_field(char *buf, size_t cap, int *n,
                         const char *s, int width, int right)
{
  int len = 0;
  int pad;
  const char *p = s ? s : "";

  while (p[len]) len++;
  if (len > width) len = width;
  pad = width - len;
  if (right) {
    while (pad-- > 0 && (size_t)(*n) + 1 < cap)
      buf[(*n)++] = ' ';
  }
  while (len-- > 0 && (size_t)(*n) + 1 < cap)
    buf[(*n)++] = *p++;
  if (!right) {
    while (pad-- > 0 && (size_t)(*n) + 1 < cap)
      buf[(*n)++] = ' ';
  }
}

static void append_uint_field(char *buf, size_t cap, int *n,
                              unsigned v, int width)
{
  char s[16];
  char tmp[16];
  int i = 0, len = 0;
  unsigned x = v;

  if (x == 0)
    tmp[i++] = '0';
  else {
    while (x && i < (int)sizeof(tmp)) {
      tmp[i++] = (char)('0' + (x % 10u));
      x /= 10u;
    }
  }
  while (i--)
    s[len++] = tmp[i];
  s[len] = '\0';
  append_field(buf, cap, n, s, width, 1);
}

static const char *state_name(STAT s)
{
  if (s == TTS_NOEXS) return "NOEXS";
  if (s & TTS_RUN) return "RUN";
  if (s & TTS_RDY) return "RDY";
  if (s & TTS_WAI) return "WAI";
  if (s & TTS_SUS) return "SUS";
  if (s & TTS_DMT) return "DMT";
  return "?";
}

static void fmt_uint(char *out, int outsz, unsigned v, int *len)
{
  char tmp[12];
  int ti = 0;
  int n = 0;

  if (outsz <= 0) {
    *len = 0;
    return;
  }
  if (v == 0)
    tmp[ti++] = '0';
  else {
    while (v && ti < (int)sizeof(tmp)) {
      tmp[ti++] = (char)('0' + (v % 10u));
      v /= 10u;
    }
  }
  while (ti-- && n + 1 < outsz)
    out[n++] = tmp[ti];
  out[n] = '\0';
  *len = n;
}

static int gen_tasks(char *buf, size_t cap)
{
  int i, n = 0;
  char stk[20];
  int sn;

#if CFG_USE_TSKNAME
  append_field(buf, cap, &n, "id", 2, 1);
  append_str(buf, cap, &n, " ");
  append_field(buf, cap, &n, "name", TSK_NAME_LEN, 0);
  append_str(buf, cap, &n, " ");
#else
  append_field(buf, cap, &n, "id", 2, 1);
  append_str(buf, cap, &n, " ");
#endif
  append_field(buf, cap, &n, "pri", 3, 1);
  append_str(buf, cap, &n, " ");
  append_field(buf, cap, &n, "state", 5, 0);
  append_str(buf, cap, &n, " ");
  append_field(buf, cap, &n, "stk", 11, 1);
  append_str(buf, cap, &n, " ");
  append_field(buf, cap, &n, "stk%", 4, 1);
  append_str(buf, cap, &n, " ");
  append_field(buf, cap, &n, "cpu%", 4, 1);
  append_str(buf, cap, &n, "\n");

  for (i = 0; i < TNUM_TSKID; i++) {
    TCB *t = &tcb_table[i];
    SIZE used;
    int ulen, slen;

    if (t->state == TTS_NOEXS) continue;
    used = task_stack_used(t);

    append_uint_field(buf, cap, &n, (unsigned)t->tskid, 2);
    append_str(buf, cap, &n, " ");
#if CFG_USE_TSKNAME
    append_field(buf, cap, &n, t->name[0] ? t->name : "-", TSK_NAME_LEN, 0);
    append_str(buf, cap, &n, " ");
#endif
    append_uint_field(buf, cap, &n, (unsigned)t->tskpri, 3);
    append_str(buf, cap, &n, " ");
    append_field(buf, cap, &n, state_name(t->state), 5, 0);
    append_str(buf, cap, &n, " ");

    sn = 0;
    fmt_uint(stk, (int)sizeof(stk), (unsigned)used, &ulen);
    sn = ulen;
    if (sn + 1 < (int)sizeof(stk))
      stk[sn++] = '/';
    fmt_uint(stk + sn, (int)sizeof(stk) - sn, (unsigned)t->stksz, &slen);
    sn += slen;
    stk[sn] = '\0';
    append_field(buf, cap, &n, stk, 11, 1);
    append_str(buf, cap, &n, " ");
    append_uint_field(buf, cap, &n, task_stack_pct(t), 4);
    append_str(buf, cap, &n, " ");
    append_uint_field(buf, cap, &n, task_cpu_pct(t), 4);
    append_str(buf, cap, &n, "\n");
    if ((size_t)n + 48 >= cap) break;
  }
  if (runtsk) {
    append_str(buf, cap, &n, "run=");
    append_uint(buf, cap, &n, (unsigned)runtsk->tskid);
    append_str(buf, cap, &n, " sched=");
    append_uint(buf, cap, &n, schedtsk ? (unsigned)schedtsk->tskid : 0);
    append_str(buf, cap, &n, "\n");
  }
  buf[n] = '\0';
  return n;
}

static int gen_ticks(char *buf, size_t cap)
{
  SYSTIM t;
  int n = 0;
  if (get_tim(&t) != E_OK) {
    append_str(buf, cap, &n, "n/a\n");
  } else {
    append_uint(buf, cap, &n, (unsigned)t.st_high);
    append_str(buf, cap, &n, ":");
    append_uint(buf, cap, &n, (unsigned)t.st_low);
    append_str(buf, cap, &n, "\n");
  }
  buf[n] = '\0';
  return n;
}

static int gen_mem(char *buf, size_t cap)
{
  char *cur = (char *)_sbrk(0);
  int n = 0;
  unsigned heap = (unsigned)(cur - &end);
  append_str(buf, cap, &n, "heap_used=");
  append_uint(buf, cap, &n, heap);
  append_str(buf, cap, &n, "\n");
  buf[n] = '\0';
  return n;
}

static int gen_stat(char *buf, size_t cap)
{
  int n = 0;
  append_str(buf, cap, &n, "wall=");
  append_uint(buf, cap, &n, (unsigned)telemetry_wall_ticks);
  append_str(buf, cap, &n, " idle=");
  append_uint(buf, cap, &n, (unsigned)telemetry_idle_ticks);
  append_str(buf, cap, &n, " idle%=");
  append_uint(buf, cap, &n, idle_cpu_pct());
  append_str(buf, cap, &n, "\n");
  buf[n] = '\0';
  return n;
}

static int gen_interrupts(char *buf, size_t cap)
{
  int n = 0;
  unsigned v;

  append_str(buf, cap, &n, "vec name count\n");
  for (v = 0; v < TELEMETRY_IRQ_MAX; v++) {
    if (irq_count[v] == 0)
      continue;
    append_uint(buf, cap, &n, v);
    append_str(buf, cap, &n, " ");
    append_str(buf, cap, &n, irq_vec_name(v));
    append_str(buf, cap, &n, " ");
    append_uint(buf, cap, &n, (unsigned)irq_count[v]);
    append_str(buf, cap, &n, "\n");
    if ((size_t)n + 32 >= cap) break;
  }
  buf[n] = '\0';
  return n;
}

static int gen_version(char *buf, size_t cap)
{
  int n = 0;
  append_str(buf, cap, &n, "ittrium RTOS services\n");
  buf[n] = '\0';
  return n;
}

static int gen_file(const char *name, char *buf, size_t cap)
{
  if (strcmp(name, "tasks") == 0) return gen_tasks(buf, cap);
  if (strcmp(name, "ticks") == 0) return gen_ticks(buf, cap);
  if (strcmp(name, "mem") == 0) return gen_mem(buf, cap);
  if (strcmp(name, "stat") == 0) return gen_stat(buf, cap);
  if (strcmp(name, "interrupts") == 0) return gen_interrupts(buf, cap);
  if (strcmp(name, "version") == 0) return gen_version(buf, cap);
  return -1;
}

static int pf_open(void *fs, const char *rel, int flags, void **out)
{
  struct proc_file *f;
  char tmp[PROC_BUF_MAX];
  int n;
  const char *name = rel ? rel : "";

  (void)fs;
  (void)flags;
  while (*name == '/')
    name++;
  if (*name == '\0')
    return -1;
  n = gen_file(name, tmp, sizeof(tmp));
  if (n < 0)
    return -1;

  f = (struct proc_file *)malloc(sizeof(*f));
  if (!f)
    return -1;
  f->buf = (char *)malloc((size_t)n + 1);
  if (!f->buf) {
    free(f);
    return -1;
  }
  memcpy(f->buf, tmp, (size_t)n);
  f->buf[n] = '\0';
  f->len = (size_t)n;
  f->pos = 0;
  *out = f;
  return 0;
}

static int pf_read(void *file, void *buf, size_t len)
{
  struct proc_file *f = (struct proc_file *)file;
  size_t left;

  if (!f || !buf)
    return -1;
  if (f->pos >= f->len)
    return 0;
  left = f->len - f->pos;
  if (len > left)
    len = left;
  memcpy(buf, f->buf + f->pos, len);
  f->pos += len;
  return (int)len;
}

static int pf_close(void *file)
{
  struct proc_file *f = (struct proc_file *)file;
  if (!f)
    return -1;
  free(f->buf);
  free(f);
  return 0;
}

static int pf_stat(void *fs, const char *rel, struct vfs_stat *st)
{
  int i;
  const char *name = rel ? rel : "";

  (void)fs;
  while (*name == '/')
    name++;
  memset(st, 0, sizeof(*st));
  if (*name == '\0') {
    st->type = VFS_TYPE_DIR;
    return 0;
  }
  for (i = 0; proc_names[i]; i++) {
    if (strcmp(proc_names[i], name) == 0) {
      st->type = VFS_TYPE_REG;
      return 0;
    }
  }
  return -1;
}

static int pf_listdir(void *fs, const char *rel, char *name, size_t nlen, int *idx)
{
  int i = *idx;

  (void)fs;
  (void)rel;
  if (!proc_names[i])
    return 1;
  strncpy(name, proc_names[i], nlen - 1);
  name[nlen - 1] = '\0';
  (*idx)++;
  return 0;
}

static const struct vfs_file_ops g_proc_ops = {
  .open = pf_open,
  .read = pf_read,
  .close = pf_close,
  .stat = pf_stat,
  .listdir = pf_listdir,
};

int procfs_mount(const char *path)
{
  return vfs_mount(path, &g_proc_ops, (void *)1);
}

const struct vfs_file_ops *procfs_ops(void)
{
  return &g_proc_ops;
}
