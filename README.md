# ittrium

**ittrium** is a compact real-time operating system based on the **µITRON** (μITRON) specification.  
The kernel is preemptive with priority-based tasks; an optional platform services layer (VFS, shell, lwIP, ELF) sits on top.

License: [GPL-3.0](LICENSE).

---

## Features

### Kernel (µITRON)

| Object | API (subset) |
|--------|----------------|
| Tasks | `cre_tsk` / `acre_tsk` / `act_tsk` / `iact_tsk` / `ext_tsk` / `del_tsk` / `chg_pri` / `rot_rdq` / `dly_tsk` / `slp_tsk` / `wup_tsk` / `sus_tsk` / `rsm_tsk` |
| Semaphores | `cre_sem` / `acre_sem` / `del_sem` / `wai_sem` / `sig_sem` / `ref_sem` (plus ISR variants `i*`) |
| Eventflags | `cre_flg` / `acre_flg` / `del_flg` / `set_flg` / `wai_flg` / `twai_flg` / `iset_flg` / `ref_flg` |
| Mailboxes | `cre_mbx` / `acre_mbx` / `del_mbx` / `snd_mbx` / `rcv_mbx` / `ref_mbx` |
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
│   ├── linux-user/    # host process (ucontext + SIGALRM)
│   ├── cortex-m3/ m4/
│   ├── arm7/ …
│   └── tms320c…/
├── services/          # optional upper layer (VFS, shell, net, elf, drv)
│   ├── services.mk    # wired by CFG_USE_*
│   ├── drv/           # console, netdev
│   ├── vfs/ proc/ shell/ net/ elf/
│   └── include/
├── third_party/
│   ├── littlefs/      # CFG_USE_LFS
│   └── lwip/          # CFG_USE_LWIP (clone upstream here)
├── example/           # boards / demos
│   ├── qemu-a53/      # QEMU virt — primary A53 bring-up
│   ├── qemu-m3/       # QEMU lm3s6965evb — Cortex-M3 smoke
│   ├── linux-user/    # host gcc smoke (no QEMU)
│   ├── kria-k26/      # Kria K26 SOM (UART1 + GEM3)
│   ├── stm32l100/ gd32f350/ c55x/ …
└── README.md
```

---

## CPU ports

Each CPU port lives under `config/<cpu>/` (`cpu_support.[chS]`, `ittrium.inc`, optional `Makefile.dev`).  
Board glue (UART, clocks, netdev) stays in `example/<board>/`.

| CPU | Directory | Toolchain | Example | Status |
|-----|-----------|-----------|---------|--------|
| Cortex-A53 (AArch64 EL1, no MMU) | `config/cortex-a53` | `aarch64-none-elf-gcc` | `example/qemu-a53`, `kria-k26` | **Primary** — QEMU smoke-tested; Kria links |
| Linux userspace | `config/linux-user` | host `gcc` | `example/linux-user` | Host smoke (ucontext + SIGALRM; no QEMU) |
| Cortex-M3 | `config/cortex-m3` | `arm-none-eabi-gcc` | `example/qemu-m3`, `stm32l100` | **QEMU smoke** (`lm3s6965evb`); STM32 board needs CMSIS/HAL |
| Cortex-M4 (+ optional FPU ctx) | `config/cortex-m4` | `arm-none-eabi-gcc` | `example/gd32f350` | Board example (needs GD32 FW lib) |
| ARM7 | `config/arm7` | IAR ARM | `example/arm7` | Legacy; IAR/EW project (no host `make` link) |
| ARM7TDMI | `config/arm7tdmi` | IAR ARM | `example/arm7tdmi` | Legacy; IAR smoke layout |
| LPC288x | `config/lpc288x` | IAR ARM | `example/lpc288x` | Legacy; IAR smoke + board stubs |
| M16C/62 | `config/m16c62` | IAR M16C | `example/m16c62` | Legacy; IAR-oriented smoke |
| TMS320 C54x | `config/tms320c54x` | TI C54x | `example/c54_test` | CCS/simulator |
| TMS320 C55x | `config/tms320c55x` | TI `cl55` | `example/c55x` | Builds with CCS 4.4.x |
| TMS320 C6x / C67x+ | `config/tms320c6x`, `tms320c67px` | TI C6000 | `example/c6x`, `c67px` | Smoke builds with CGT 8.x (`-mv6740` ELF) |

A new CPU means a new `config/` directory plus an `example/`.  
A new board on an existing CPU is usually only `example/<board>/` (see [Porting](#porting)).

**Port layout (typical):**

```
config/<cpu>/
├── cpu_support.h    # types, CTXB, cpu_lock/unlock, dispatch()
├── cpu_support.c    # make_task_context, IRQ table, tick
├── cpu_support.S    # context switch / IRQ entry (or .s / .asm)
├── ittrium.inc      # TCB field offsets for asm
└── Makefile.dev     # CROSS_COMPILE flags (GNU ports)
```

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
- **lwIP** under `third_party/lwip` (when `CFG_USE_LWIP=1`)

For **linux-user** (native `gcc`, no QEMU):

```bash
cd example/linux-user && make && make run
# Ctrl+C to stop
```

Details: [`example/linux-user/README.md`](example/linux-user/README.md).

For **qemu-m3** (Cortex-M3 shell, no ST HAL):

```bash
git clone --depth 1 -b 5.9.0 \
  https://github.com/ARM-software/CMSIS_5.git third_party/CMSIS_5
cd example/qemu-m3 && make && make run
# ittrium> on the QEMU console
```

Details: [`example/qemu-m3/README.md`](example/qemu-m3/README.md).

---

## Quick start: QEMU (Cortex-A53)

```bash
git clone <this-repo> ittrium
git clone https://git.savannah.nongnu.org/git/lwip.git ittrium/third_party/lwip   # or your mirror

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
make          # → hello.elf (also embedded into qemu image as /hello.elf)
```

- Linker script: [`services/elf/app.ld`](services/elf/app.ld)  
- Flags: `-fPIE -pie -Bsymbolic` (see app `Makefile`)  
- Loader slides the image and applies `R_AARCH64_RELATIVE` / `R_AARCH64_ABS64`  
- On QEMU: `make run`, then `run /hello.elf`  

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
| lwIP | `third_party/lwip` | when `CFG_USE_LWIP`; override with `LWIPDIR` (=`…/lwip/src`) |

---

## Example docs

- [`example/qemu-a53/README.md`](example/qemu-a53/README.md) — QEMU virt, virtio-net, shell  
- [`example/qemu-m3/README.md`](example/qemu-m3/README.md) — QEMU Stellaris LM3S6965, Cortex-M3 smoke  
- [`example/linux-user/README.md`](example/linux-user/README.md) — host process smoke  
- [`example/kria-k26/README.md`](example/kria-k26/README.md) — Kria K26 SOM / GEM3 / UART1  
- [`example/stm32l100/README.md`](example/stm32l100/README.md) — STM32L100 / Renode shell (USART1)  
- [`example/gd32f350/README.md`](example/gd32f350/README.md) — Cortex-M4 USB CDC  
- [`example/c55x/README.md`](example/c55x/README.md) — TMS320 C55x eventflag + soft IRQ  
- [`example/c54_test/README.md`](example/c54_test/README.md) — TMS320 C54x minimal stub  
- [`example/c6x/README.md`](example/c6x/README.md) — TMS320 C6x smoke (`cl6x`)  
- [`example/c67px/README.md`](example/c67px/README.md) — TMS320 C67x+ smoke (`cl6x`)  
- [`example/arm7/README.md`](example/arm7/README.md) — ARM7 IAR smoke  
- [`example/arm7tdmi/README.md`](example/arm7tdmi/README.md) — ARM7TDMI IAR smoke  
- [`example/lpc288x/README.md`](example/lpc288x/README.md) — LPC288x IAR smoke  
- [`example/m16c62/README.md`](example/m16c62/README.md) — M16C/62 IAR smoke  

---

## Out of scope (for now)

- MMU / userspace / SMP on A53  
- Dynamic linker, POSIX signals  
- Full QSPI block device (littlefs often uses a RAM block device today)  
- Reusing the Xilinx no-OS BSP (EL3+MMU) — register reference only  

---

## License

GNU GPL v3 — see [LICENSE](LICENSE).
