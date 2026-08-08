# STM32L100 — Cortex-M3

Minimal board example for `config/cortex-m3`: eventflag wait with timeout, a soft IRQ that sets the flag, and GPIO toggle on **PB12**.

## Dependencies

```bash
export CMSIS_ARM_PATH=<path to https://github.com/ARM-software/CMSIS_5>
export CMSIS_DEV_PATH=<path to https://github.com/STMicroelectronics/cmsis_device_l1>
export HAL_PATH=<path to https://github.com/STMicroelectronics/stm32l1xx_hal_driver>
```

Toolchain: `arm-none-eabi-gcc` (`CROSS_COMPILE` defaults to `arm-none-eabi-`).

## Build

```bash
cd example/stm32l100
make clean
make
```

Artifact: `test_stm32l1x.out` (linker script `lnk.ld`).

## What it does

- `init_tsk` creates an eventflag + `test_tsk`, installs IRQ vector `TEST_INT_VEC_NO`
- `test_tsk` waits on the flag (`twai_flg`, 1 s timeout) and toggles `GPIOB` pin 12
- Clock/GPIO setup in `_low_level_init()` / `target_clock_init()` (HSI + PLL)

No VFS/shell/lwIP — kernel-only smoke for the M3 port.

## Related

- CPU port: [`config/cortex-m3/`](../../config/cortex-m3/)
- Similar M4 USB example: [`../gd32f350/`](../gd32f350/)
