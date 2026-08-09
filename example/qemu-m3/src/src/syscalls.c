#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

char *__env[1] = { 0 };
char **environ = __env;

void initialise_monitor_handles(void)
{
}

int _getpid(void)
{
	return 1;
}

int _kill(int pid, int sig)
{
	(void)pid;
	(void)sig;
	errno = EINVAL;
	return -1;
}

void _exit(int status)
{
	_kill(status, -1);
	for (;;)
		;
}

int _read(int file, char *ptr, int len)
{
	(void)file;
	(void)ptr;
	(void)len;
	return 0;
}

int _write(int file, char *ptr, int len)
{
	(void)file;
	int i;

	for (i = 0; i < len; i++)
		__io_putchar(ptr[i]);
	return len;
}

int _close(int file)
{
	(void)file;
	return -1;
}

int _fstat(int file, struct stat *st)
{
	(void)file;
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	(void)file;
	return 1;
}

int _lseek(int file, int ptr, int dir)
{
	(void)file;
	(void)ptr;
	(void)dir;
	return 0;
}

int _open(char *path, int flags, ...)
{
	(void)path;
	(void)flags;
	return -1;
}

extern char end;
static char *heap_end;

void *_sbrk(int incr)
{
	char *prev;

	if (heap_end == 0)
		heap_end = &end;
	prev = heap_end;
	heap_end += incr;
	return prev;
}
