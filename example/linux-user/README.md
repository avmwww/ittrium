# Linux userspace

Host smoke for `config/linux-user`: ittrium as a process (`ucontext` switch, `SIGALRM` tick). No cross toolchain, no QEMU.

## Build & run

```bash
cd example/linux-user
make
make run
```

Needs host `gcc` / `make`. littlefs under `third_party/littlefs` when `CFG_USE_LFS=1`.

Stop with **Ctrl+C** (restores tty).

## What boots

- kernel tasks / 1 ms tick
- VFS: romfs `/`, littlefs `/data`, procfs `/proc`
- shell on stdin/stdout (soft RX FIFO + eventflag; poll from tick / `SIGIO`)

Off by default: lwIP, ELF, sysfs (AArch64-only).

## Shell

```
help
ps
irq
ls /
cat /proc/tasks
source /demo.sh
```

## Notes

- Soft IRQs do not preempt via signal `ucontext` (unsafe with `swapcontext`); ready tasks run on the next `dispatch` / idle `pause`.
- `CFG_USE_*` in `include/kernel_config.h`.

## Related

- CPU port: [`config/linux-user/`](../../config/linux-user/)
- A53 QEMU reference: [`../qemu-a53/`](../qemu-a53/)
