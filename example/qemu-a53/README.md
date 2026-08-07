# QEMU virt — Cortex-A53 AArch64 EL1 (no MMU) + platform services

## Build

```bash
export PATH=/opt/xilinx/Vitis/2023.2/gnu/aarch64/lin/aarch64-none/bin:$PATH
cd example/qemu-a53
make
make run
```

## What boots

- ittrium kernel (tasks / ticks)
- VFS: romfs `/`, ramfs `/data`, procfs `/proc`, sysfs `/sys`
- lwIP (`../lwip`) with `NO_SYS=0` sys_arch, loopback, TCP echo `:7`
- shell (cmd-table + line editor) on UART (IRQ + eventflag + soft RX/TX FIFO)

## Shell examples

```
help
ps
ticks
ls /
cat /readme.txt
mount
cat /proc/tasks
cat /sys/cpu/el
run /data/app.elf   # ELF/ITRM loader
```

## Notes

- lwIP tree expected at `ittrium/../lwip`
- littlefs port is in `services/vfs/lfs_port.c` but currently unused (hang in `lfs_mount`); `/data` is ramfs for now
- Next: virtio-net, littlefs fix, kria_rtx bring-up
