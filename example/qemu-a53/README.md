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
- VFS: romfs `/`, littlefs `/data`, procfs `/proc`, sysfs `/sys`
- lwIP (`../lwip`) with `NO_SYS=0` sys_arch, virtio-net eth `10.0.2.15`, TCP echo `:7` (host `:10007`)
- shell (cmd-table + line editor) on UART (IRQ + eventflag + soft RX/TX FIFO)

## Shell examples

```
help
ps
irq
load
ifconfig
arp
ping 10.0.2.2
ls /
cat /readme.txt
mount
cat /proc/tasks
cat /proc/interrupts
cat /sys/cpu/load
run /data/app.elf   # ELF/ITRM loader
```

## Notes

- lwIP tree expected at `ittrium/../lwip`
- littlefs: RAM block device (`services/vfs/lfs_port.c`); format-on-first-mount
- Driver glue: `services/drv` (`console`, `netdev`); board UART/virtio register ports
- Services via `CFG_USE_*` in `include/kernel_config.h` (0 = omit)
- `make run` uses modern virtio-mmio (`force-legacy=false`) on `virtio-mmio-bus.0` + user-net hostfwd
- Kria port: [`../kria-rtx/`](../kria-rtx/) (UART1 + GEM3, EL1 no-MMU)