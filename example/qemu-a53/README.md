# QEMU virt — Cortex-A53 AArch64 EL1 (no MMU)

Reference board for ittrium on **QEMU `virt`**: kernel + platform services (VFS, shell, lwIP, virtio-net).

## Build & run

```bash
cd example/qemu-a53
make
make run
# or from repo root: ./qrun.sh
```

Requires `aarch64-none-elf-gcc` on `PATH` (or `CROSS_COMPILE=…`).  
Also requires an lwIP tree at `ittrium/../lwip` when `CFG_USE_LWIP=1`.

`make run` starts:

- machine `virt`, GICv2, Cortex-A53, 128 MiB RAM  
- modern virtio-mmio (`-global virtio-mmio.force-legacy=false`) on `virtio-mmio-bus.0`  
- user-net with `hostfwd=tcp::10007-:7`

## What boots

- ittrium kernel (tasks / ticks)
- VFS: romfs `/`, littlefs `/data`, procfs `/proc`, sysfs `/sys`
- lwIP (`NO_SYS=0` sys_arch), virtio-net guest `10.0.2.15`, TCP echo on `:7`
- shell (command table + line editor) on PL011 UART (IRQ + eventflag + soft RX/TX FIFO)

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
run /data/app.elf
```

## Loadable apps (ELF)

`make` builds `app/hello.elf` and embeds it in romfs as `/hello.elf`.

```bash
cd example/qemu-a53 && make && make run
# in shell:
ls /
run /hello.elf
ps
```

App sources: `example/qemu-a53/app/` (linker script `services/elf/app.ld`, PIE + reloc).

## Networking from the host

QEMU **user-net (SLIRP)** does not forward ICMP host→guest, so `ping 10.0.2.15` from the host will not work.

| Direction | How |
|-----------|-----|
| Guest → gateway | `ping 10.0.2.2` in the shell |
| Host → guest TCP echo | `nc 127.0.0.1 10007` (forwarded to guest `:7`) |
| Host ↔ guest ICMP | needs tap/bridge networking, not user-net |

## Notes

- littlefs uses a RAM block device (`services/vfs/lfs_port.c`); format on first mount
- Driver glue: `services/drv` (`console`, `netdev`); this board owns PL011 + virtio-net
- Enable/disable services with `CFG_USE_*` in `include/kernel_config.h` (`0` = omit)
- Related hardware port: [`../kria-k26/`](../kria-k26/) (Kria K26 SOM, UART1 + GEM3, same EL1 no-MMU model)
