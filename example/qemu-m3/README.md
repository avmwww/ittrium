# QEMU Stellaris LM3S6965 — Cortex-M3 shell for `config/cortex-m3`

No ST HAL / CMSIS-Device. Needs CMSIS Core from upstream CMSIS_5.

## Dependencies

```bash
# from repo root (default path for CMSIS_ARM_PATH)
git clone --depth 1 -b 5.9.0 \
  https://github.com/ARM-software/CMSIS_5.git third_party/CMSIS_5
```

Or point elsewhere:

```bash
export CMSIS_ARM_PATH=/path/to/CMSIS_5
```

Toolchain: `arm-none-eabi-gcc`, `qemu-system-arm`.

## Build & run

```bash
cd example/qemu-m3
make
make run
```

UART0 is attached to the terminal (`-nographic`). At `ittrium>` type commands (`help`, `ps`, `ls /`, …).

Stop with **Ctrl+A X** (QEMU), or Ctrl+C.

## What boots

- SysTick 1 ms
- UART0: IRQ + eventflag + soft RX/TX FIFO → console
- VFS + romfs (`/`) + procfs (`/proc`)
- shell task stays interactive on the QEMU console

## Related

- CPU port: [`config/cortex-m3/`](../../config/cortex-m3/)
- Board (needs CMSIS/HAL): [`../stm32l100/`](../stm32l100/)
- A53 QEMU: [`../qemu-a53/`](../qemu-a53/)
