#include "procfs.h"
#include "task.h"
#include <stdlib.h>
#include <string.h>

#define PROC_BUF_MAX 512

struct proc_file {
  char *buf;
  size_t len;
  size_t pos;
};

static const char *const proc_names[] = {
  "tasks", "ticks", "mem", "version", 0
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

static int gen_tasks(char *buf, size_t cap)
{
  int i, n = 0;
  append_str(buf, cap, &n, "id pri state\n");
  for (i = 0; i < TNUM_TSKID; i++) {
    TCB *t = &tcb_table[i];
    if (t->state == TTS_NOEXS) continue;
    append_uint(buf, cap, &n, (unsigned)t->tskid);
    append_str(buf, cap, &n, " ");
    append_uint(buf, cap, &n, (unsigned)t->tskpri);
    append_str(buf, cap, &n, " ");
    append_str(buf, cap, &n, state_name(t->state));
    append_str(buf, cap, &n, "\n");
    if ((size_t)n + 8 >= cap) break;
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
  append_str(buf, cap, &n, "heap_end ok\nused=");
  append_uint(buf, cap, &n, (unsigned)(cur - &end));
  append_str(buf, cap, &n, "\n");
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
  if (strcmp(name, "version") == 0) return gen_version(buf, cap);
  return -1;
}

static int pf_open(void *fs, const char *rel, int flags, void **out)
{
  struct proc_file *f;
  char tmp[PROC_BUF_MAX];
  int n;
  const char *name = rel ? rel : "";

  (void)fs; (void)flags;
  while (*name == '/') name++;
  if (*name == '\0') return -1;

  n = gen_file(name, tmp, sizeof(tmp));
  if (n < 0) return -1;

  f = (struct proc_file *)malloc(sizeof(*f));
  if (!f) return -1;
  f->buf = (char *)malloc((size_t)n + 1);
  if (!f->buf) { free(f); return -1; }
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
  if (!f || !f->buf) return -1;
  if (f->pos >= f->len) return 0;
  left = f->len - f->pos;
  if (len > left) len = left;
  memcpy(buf, f->buf + f->pos, len);
  f->pos += len;
  return (int)len;
}

static int pf_write(void *file, const void *buf, size_t len)
{
  (void)file; (void)buf; (void)len;
  return -1;
}

static int pf_close(void *file)
{
  struct proc_file *f = (struct proc_file *)file;
  if (f) {
    free(f->buf);
    free(f);
  }
  return 0;
}

static int pf_stat(void *fs, const char *rel, struct vfs_stat *st)
{
  const char *name = rel ? rel : "";
  int i;
  (void)fs;
  while (*name == '/') name++;
  if (*name == '\0') {
    st->type = VFS_TYPE_DIR;
    st->size = 0;
    return 0;
  }
  for (i = 0; proc_names[i]; i++) {
    if (strcmp(proc_names[i], name) == 0) {
      st->type = VFS_TYPE_REG;
      st->size = 0;
      return 0;
    }
  }
  return -1;
}

static long pf_lseek(void *file, long off, int whence)
{
  struct proc_file *f = (struct proc_file *)file;
  long np;
  if (!f) return -1;
  if (whence == VFS_SEEK_SET) np = off;
  else if (whence == VFS_SEEK_CUR) np = (long)f->pos + off;
  else if (whence == VFS_SEEK_END) np = (long)f->len + off;
  else return -1;
  if (np < 0) return -1;
  if ((size_t)np > f->len) np = (long)f->len;
  f->pos = (size_t)np;
  return np;
}

static int pf_listdir(void *fs, const char *rel, char *name, size_t nlen, int *idx)
{
  const char *p = rel ? rel : "";
  int i;
  (void)fs;
  while (*p == '/') p++;
  if (*p != '\0') return -1;
  if (!idx || *idx < 0) return -1;
  i = *idx;
  if (!proc_names[i]) return 1;
  strncpy(name, proc_names[i], nlen - 1);
  name[nlen - 1] = '\0';
  *idx = i + 1;
  return 0;
}

static const struct vfs_file_ops g_proc_ops = {
  pf_open, pf_read, pf_write, pf_close, pf_stat, pf_lseek, pf_listdir
};

int procfs_mount(const char *path)
{
  return vfs_mount(path, &g_proc_ops, (void *)1);
}
