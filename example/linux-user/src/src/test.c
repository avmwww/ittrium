/**
 * linux-user init (CFG_USE_* in kernel_config.h)
 */
#include "ittrium.h"
#include "target.h"
#include "drv/console.h"

#if CFG_USE_SHELL
#include "shell.h"
#endif
#if CFG_USE_VFS
#include "vfs.h"
#endif
#if CFG_USE_ROMFS
#include "romfs.h"
#endif
#if CFG_USE_LFS
#include "lfs_port.h"
#endif
#if CFG_USE_RAMFS
#include "ramfs.h"
#endif
#if CFG_USE_PROCFS
#include "procfs.h"
#endif

uint64_t init_tsk_stack[INIT_TASK_STACK_SIZE / sizeof(uint64_t)];
#if CFG_USE_SHELL
uint64_t shell_tsk_stack[SHELL_TSK_STACK_SIZE / sizeof(uint64_t)];
#endif

#if CFG_USE_SHELL
static void start_shell(void)
{
	T_CTSK pk;

	shell_set_io(console_putc, console_getc);

	pk.tskatr = TA_HLNG | TA_NAME;
	pk.exinf = 0;
	pk.task = (FP)shell_task;
	pk.itskpri = SHELL_TASK_PRIO;
	pk.stksz = SHELL_TSK_STACK_SIZE;
	pk.stk = shell_tsk_stack;
	pk.name = "shell";

	cre_tsk(SHELL_TASK_ID, &pk);
	act_tsk(SHELL_TASK_ID);
}
#endif

#if CFG_USE_LFS
static void lfs_smoke(void)
{
	int fd, n;
	char buf[16];

	fd = vfs_open("/data/smoke.txt", VFS_O_WRONLY | VFS_O_CREAT | VFS_O_TRUNC);
	if (fd < 0) {
		console_puts("lfs: smoke open-w failed\n");
		return;
	}
	n = vfs_write(fd, "ok\n", 3);
	vfs_close(fd);
	if (n != 3) {
		console_puts("lfs: smoke write failed\n");
		return;
	}

	fd = vfs_open("/data/smoke.txt", VFS_O_RDONLY);
	if (fd < 0) {
		console_puts("lfs: smoke open-r failed\n");
		return;
	}
	n = vfs_read(fd, buf, sizeof(buf) - 1);
	vfs_close(fd);
	if (n != 3 || buf[0] != 'o' || buf[1] != 'k') {
		console_puts("lfs: smoke read failed\n");
		return;
	}
	console_puts("lfs: smoke ok\n");
}
#endif

void init_tsk(void *exinf)
{
	(void)exinf;

	uart_init();
	console_puts("ittrium linux-user\n");

#if CFG_USE_VFS
	console_puts("vfs...\n");
	vfs_init();
#endif
#if CFG_USE_ROMFS
	console_puts("romfs...\n");
	{
		static const char demo_sh[] =
			"# ittrium demo script\n"
			"echo hello from demo.sh\n"
			"set DEMO=1\n"
			"echo DEMO=$DEMO\n"
			"ls /\n";
		if (romfs_add_builtin("demo.sh", demo_sh, sizeof(demo_sh) - 1) != 0)
			console_puts("romfs: demo.sh add failed\n");
	}
	romfs_mount("/", NULL, 0);
#endif
#if CFG_USE_PROCFS
	console_puts("proc...\n");
	procfs_mount("/proc");
#endif
#if CFG_USE_LFS
	console_puts("data...\n");
	if (lfs_port_mount("/data") != 0)
		console_puts("lfs: mount failed\n");
	else {
		console_puts("lfs: /data ready\n");
		lfs_smoke();
	}
#elif CFG_USE_RAMFS
	console_puts("data...\n");
	if (ramfs_mount("/data") != 0)
		console_puts("ramfs: mount failed\n");
	else
		console_puts("ramfs: /data ready\n");
#endif

#if CFG_USE_SHELL
	start_shell();
#endif
	chg_pri(TSK_SELF, LOW_PRIO);

	for (;;)
		dly_tsk(10000);
}

void _low_level_init(void)
{
}
