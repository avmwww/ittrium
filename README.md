# ittrium

**ittrium** is a compact real-time operating system based on the **µITRON** (μITRON) specification.  
The kernel is preemptive with priority-based tasks; an optional platform services layer (VFS, shell, lwIP, ELF) sits on top.

License: [GPL-3.0](LICENSE).

---

## Features

### Kernel (µITRON)

| Object | API (subset) |
|--------|----------------|
| Tasks | `cre_tsk` / `act_tsk` / `ext_tsk` / `chg_pri` / `dly_tsk` / `sus_tsk` / `rsm_tsk` |
| Semaphores | `cre_sem` / `wai_sem` / `sig_sem` (plus ISR variants `i*`) |
| Eventflags | `cre_flg` / `set_flg` / `wai_flg` / `twai_flg` / `iset_flg` |
| Mailboxes | `cre_mbx` / `snd_mbx` / `rcv_mbx` |
| Time | `set_tim` / `get_tim`, system tick |

Context switch, dispatch, and IRQ handling live in the CPU port (`config/<cpu>/`).

### Services (`services/`, enabled via `CFG_USE_*`)

| Flag | Provides |
|------|----------|
| `CFG_USE_VFS` | Virtual FS: mount / open / read / write |
| `CFG_USE_ROMFS` | Read-only image at `/` |
| `CFG_USE_LFS` | littlefs (RW), typically `/data` |
| `CFG_USE_RAMFS` | RAM FS instead of LFS |
| `CFG_USE_PROCFS` | `/proc` (tasks, interrupts, stat, …) |
| `CFG_USE_SYSFS` | `/sys` (cpu: el, midr, load, …) |
| `CFG_USE_SHELL` | Command shell + line editor |
| `CFG_USE_ELF` | Load/run static AArch64 ELF from the FS (`run`); prefer PIE + `services/elf/app.ld` |
| `CFG_USE_LWIP` | lwIP (`NO_SYS=0`) + shell `ifconfig` / `arp` / `ping` |

Shared driver upper layer (board-independent):

- **`console`** — putc/getc for shell and logging  
- **`netdev`** — Ethernet frame TX/RX for lwIP  

SoC register-level code lives only under `example/<board>/`.

### AArch64 policy

The current **Cortex-A53** port runs at **EL1 with no MMU** (flat physical addressing).  
MMU / userspace / SMP are out of scope for now.

---

## Repository layout

```
ittrium/
├── include/           # µITRON API (ittrium.h, service_call.h, …)
├── kernel/            # shared kernel (tasks, sem, flg, timer, …)
├── config/            # CPU ports
│   ├── cortex-a53/    # AArch64 EL1, GICv2, CNTP
│   ├── cortex-m3/ m4/
│   ├── arm7/ …
│   └── tms320c…/
├── services/          # optional upper layer (VFS, shell, net, elf, drv)
│   ├── services.mk    # wired by CFG_USE_*
│   ├── drv/           # console, netdev
│   ├── vfs/ proc/ shell/ net/ elf/
│   └── include/
├── third_party/       # littlefs (and similar)
├── example/           # boards / demos
│   ├── qemu-a53/      # QEMU virt — primary A53 bring-up
│   ├── kria-k26/      # Kria K26 SOM (UART1 + GEM3)
│   ├── stm32l100/ gd32f350/ c55x/ …
└── README.md
```

External (expected next to the tree for networked examples):

```
../lwip/               # upstream lwIP (services.mk → LWIPDIR=…/lwip/src)
```

---

## CPU ports

| CPU | Directory | Example |
|-----|-----------|---------|
| Cortex-A53 (AArch64 EL1) | `config/cortex-a53` | `example/qemu-a53`, `example/kria-k26` |
| Cortex-M3 / M4 | `config/cortex-m3`, `cortex-m4` | `example/stm32l100`, `gd32f350` |
| ARM7 / LPC288x | `config/arm7`, `lpc288x` | — |
| M16C | `config/m16c62` | — |
| TMS320 C54x / C55x / C67x | `config/tms320c*` | `example/c55x`, `c54_test` |

A new CPU means a new `config/` directory plus an `example/`.  
A new board on an existing CPU is usually only `example/<board>/` (see [Porting](#porting)).

---

## Build requirements

- GNU Make  
- Cross compiler for the target CPU  

For **A53**, put `aarch64-none-elf-gcc` on your `PATH` (or set `CROSS_COMPILE`):

```bash
# default in Makefiles: CROSS_COMPILE=aarch64-none-elf-
export CROSS_COMPILE=aarch64-none-elf-   # optional override
```

For **qemu-a53** also:

- `qemu-system-aarch64`  
- an **lwIP** tree at `ittrium/../lwip` (when `CFG_USE_LWIP=1`)

---

## Quick start: QEMU (Cortex-A53)

```bash
git clone <this-repo> ittrium
# lwIP beside the repo:
git clone https://git.savannah.nongnu.org/git/lwip.git ../lwip   # or your mirror

cd ittrium/example/qemu-a53
make
make run
# or from the repo root: ./qrun.sh
```

`make run` starts `virt` + GICv2 + modern virtio-net + user-net (`hostfwd` TCP `:10007` → guest `:7`).

Expected boot log: VFS/romfs/proc/sys/lfs → virtio-net → lwIP → `ittrium>` prompt.

### Shell (qemu / kria)

```
help
ps
irq
load
ifconfig
arp
ping 10.0.2.2          # qemu user-net gateway
ls /
cat /proc/tasks
cat /proc/interrupts
cat /sys/cpu/load
run /data/app.elf
```

**Note:** QEMU user-net (SLIRP) does not forward ICMP from the host to the guest.  
From the host, use TCP instead, e.g. `nc 127.0.0.1 10007` (echo on guest port 7).  
Guest → gateway `ping 10.0.2.2` works. Real host↔guest ping needs tap networking.

Board details: [`example/qemu-a53/README.md`](example/qemu-a53/README.md).

### Loadable ELF apps (PIE + reloc)

```bash
cd example/qemu-a53/app
make          # → hello.elf (ET_DYN, R_AARCH64_RELATIVE)
```

- Linker script: [`services/elf/app.ld`](services/elf/app.ld)  
- Flags: `-fPIE -pie -Bsymbolic` (see app `Makefile`)  
- Loader slides the image and applies `R_AARCH64_RELATIVE` / `R_AARCH64_ABS64`  
- Copy to guest `/data`, then: `run /data/hello.elf`  

Legacy ET_EXEC in `[ELF_LOAD_BASE, +ELF_LOAD_SIZE)` needs no reloc — prefer PIE.

---

## Kria K26 SOM

```bash
cd example/kria-k26
make
# → ittrium-kria-k26.elf  (load address 0x00100000)
```

- EL1, no MMU (same model as QEMU)  
- Cadence **UART1** → `console`  
- Cadence **GEM3** → `netdev` + lwIP  
- GIC Group1 (EL1 NS)  
- Assumes FSBL / `psu_init` already set clocks, MIO, and PHY reset  
- PHY / Ethernet wiring depends on the **carrier**; defaults suit a typical K26 RGMII setup  

See [`example/kria-k26/README.md`](example/kria-k26/README.md).

---

## Porting

Two levels: **CPU** (rare) and **board** (common). Below is the usual path — a new SoC on an existing `cortex-a53` port.

### 1. Copy a reference board

```bash
cp -a example/qemu-a53 example/myboard
# or example/kria-k26 if closer to ZynqMP / Kria
```

### 2. `include/target.h` — hardware only

Define MMIO/IRQs and declare driver entry points:

```c
#define GICD_BASE     …
#define GICC_BASE     …
#define GIC_USE_GROUP1  0   /* 1 on ZynqMP EL1 NS, 0 on QEMU virt */
#define UART_BASE     …
#define UART_IRQ      …
#define TIMER_IRQ     …     /* CNTP PPI, typically 30 */
/* net: VIRTIO_NET_IRQ or GEM_IRQ — see irq_handle in cpu_support.c */

void uart_init(void);
int  my_eth_init(void);
struct netdev *my_eth_dev(void);
```

The kernel reads `GICD_BASE` / `GICC_BASE` / `TIMER_IRQ` from `target.h`.

### 3. `include/kernel_config.h` — objects and services

- Task / sem / flg limits (ID pools for lwIP, shell, …)  
- `CFG_USE_*` — what `services/services.mk` compiles in  
- Stack sizes and priorities for init / shell / net  

Disable a service with `#define CFG_USE_XXX 0` (or omit the define) so it is not linked.

### 4. Boot + linker

| File | Role |
|------|------|
| `boot.S` | Drop to EL1 if needed, MMU/cache off, clear BSS, call `main` |
| `lnk.ld` | `ORIGIN` = load address (`0x40000000` for QEMU `-kernel`, `0x00100000` for Kria DDR, …) |

### 5. Console port (UART)

Implement `console_ops` and register:

```c
static const struct console_ops ops = {
  .putc = uart_putc,
  .getc = uart_getc,
};
console_register(&ops);  /* usually at the end of uart_init() */
```

IRQ pattern (as in qemu/kria): HW FIFO → soft FIFO → `iset_flg` → task / `twai_flg` in getc/putc.  
Soft vector: `install_handler(uart_irq, UART_VEC_NO, prio)` + `gic_enable_irq(UART_IRQ, prio)`.

### 6. Netdev port (Ethernet), if using lwIP

```c
static const struct netdev_ops ops = {
  .init  = …,
  .start = …,
  .tx    = …,   /* raw Ethernet frame */
  /* .set_mac optional */
};
netdev_register(&dev);
netdev_init(&dev);
netdev_start(&dev);
/* on RX: dev->rx(dev, frame, len, dev->rx_arg); */
```

In `init_tsk`:

```c
net_init();
netif_netdev_add(&eth, my_eth_dev(), &ip, &mask, &gw);
netif_set_default(&eth);
```

Net IRQs map to soft-vec `NETDEV_VEC_NO` via `#define VIRTIO_NET_IRQ` or `#define GEM_IRQ` in `target.h` (see `config/cortex-a53/cpu_support.c`).

### 7. Example build layout

Same shape as qemu-a53:

```
example/myboard/
├── Makefile (+ TARGET)
├── Makefile.rules   # KERNEL_DIR, KERNEL_DEV=cortex-a53
├── lnk.ld, boot.S
├── include/target.h, kernel_config.h
├── kernel/Makefile  # ittrium.c + cpu_support + boot.S
└── src/
    ├── Makefile     # app SRCS + include services.mk
    └── src/         # test.c, uart.c, eth.c, syscalls.c, …
```

```bash
cd example/myboard && make
```

### 8. Smoke checklist

1. Tick / two tasks / `dly_tsk`  
2. UART: print + shell `help`  
3. `ps` / `irq` / `load` (telemetry)  
4. With lwIP: `ifconfig`, `ping`, TCP echo `:7`  

### New CPU (not just a board)

You need a full port under `config/<cpu>/`:

- `cpu_support.h` / `.c` / `.S` — context, dispatch, IRQ stub, timer  
- `Makefile.dev` — compiler flags  
- Types and critical-section macros for the architecture  

Reference: `config/cortex-a53/` plus a working `example/qemu-a53`.

---

## Driver model (summary)

```
shell / VFS / lwIP
        │
   console / netdev API
        │
   g_ittrium_devices     ← single device pool (class + name + drv)
        │
   uart.c / virtio_net.c / gem_net.c   ← example/<board> only
```

Rules:

- the only shared global for I/O drivers is `g_ittrium_devices` (`services/drv/device.c`);
- `console_register` / `netdev_register` add entries to that pool;
- services must **not** hard-code `UART_BASE` / GIC SPI numbers;
- a new SoC = new lower half + `target.h` / `kernel_config.h`;
- QEMU and Kria K26 share the same upper stack.

---

## Telemetry

With `CFG_USE_PROCFS` / `CFG_USE_SYSFS`:

| Path | Content |
|------|---------|
| `/proc/tasks` | id, pri, state, stack used/%, cpu% |
| `/proc/interrupts` | soft-vec counters (timer, uart, net) |
| `/proc/stat` | aggregates |
| `/sys/cpu/*` | el, cntfrq, midr, mmu, load |

Shell: `ps`, `irq`, `load`.

---

## Third-party dependencies

| Component | Location | Notes |
|-----------|----------|-------|
| littlefs | `third_party/littlefs` | when `CFG_USE_LFS` |
| lwIP | `../lwip` (outside the repo) | when `CFG_USE_LWIP`; override with `LWIPDIR` |

---

## Example docs

- [`example/qemu-a53/README.md`](example/qemu-a53/README.md) — QEMU virt, virtio-net, shell  
- [`example/kria-k26/README.md`](example/kria-k26/README.md) — Kria K26 SOM / GEM3 / UART1  

- [`example/stm32l100/README.md`](example/stm32l100/README.md), [`example/gd32f350/README.md`](example/gd32f350/README.md) — Cortex-M  

---

## Out of scope (for now)

- MMU / userspace / SMP on A53  
- Dynamic linker, POSIX signals  
- Full QSPI block device (littlefs often uses a RAM block device today)  
- Reusing the Xilinx no-OS BSP (EL3+MMU) — register reference only  

---

## License

GNU GPL v3 — see [LICENSE](LICENSE).
