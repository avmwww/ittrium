# STM32L100 — Cortex-M3

Shell + VFS/romfs/procfs for `config/cortex-m3`: USART1 (IRQ + eventflag + soft FIFO), GPIO **PB12**.

## Dependencies

From repo root (defaults for `CMSIS_ARM_PATH` / `CMSIS_DEV_PATH` / `HAL_PATH`):

```bash
git clone --depth 1 -b 5.9.0 \
  https://github.com/ARM-software/CMSIS_5.git third_party/CMSIS_5
git clone --depth 1 \
  https://github.com/STMicroelectronics/cmsis_device_l1.git third_party/cmsis_device_l1
git clone --depth 1 \
  https://github.com/STMicroelectronics/stm32l1xx_hal_driver.git third_party/stm32l1xx_hal_driver
```

Toolchain: `arm-none-eabi-gcc`. Emulator: Renode (QEMU has no STM32L100).

## Build

```bash
cd example/stm32l100
make
```

Artifact: `test_stm32l1x.out`.

## Interactive shell (Renode)

```bash
# portable Renode (example default path)
curl -fsSL -o /tmp/renode.tgz \
  https://github.com/renode/renode/releases/download/v1.15.3/renode-1.15.3.linux-portable.tar.gz
tar -xzf /tmp/renode.tgz -C third_party

cd example/stm32l100
make run
```

USART1 is attached to this terminal via `socat` (same idea as `qemu -nographic`). **Ctrl+C** stops Renode.

If a leftover process remains: `make stop`.

Override: `make run RENODE=/path/to/renode UART_PORT=3456`.

## Automated smoke

```bash
make run-renode
# injects help / ps / ls / → uart.log → shell smoke ok
```

## What boots

- SysTick 1 ms (HSI 16 MHz; no HAL_RCC — tick owned by ittrium)
- USART1: IRQ + `UART_FLG_ID` + soft RX/TX FIFO → console
- VFS + romfs (`/`) + procfs (`/proc`)
- shell task: prompt `ittrium>`

## Related

- CPU port: [`config/cortex-m3/`](../../config/cortex-m3/)
- QEMU M3 (no HAL): [`../qemu-m3/`](../qemu-m3/)
- Similar M4 USB example: [`../gd32f350/`](../gd32f350/)
