# STM32L100 example

Cortex-M3 board example using CMSIS and STM32L1 HAL.

## Dependencies

Set these environment variables to local clones:

```bash
export CMSIS_ARM_PATH=<path to https://github.com/ARM-software/CMSIS_5>
export CMSIS_DEV_PATH=<path to https://github.com/STMicroelectronics/cmsis_device_l1>
export HAL_PATH=<path to https://github.com/STMicroelectronics/stm32l1xx_hal_driver>
```

## Build

```bash
cd example/stm32l100
make clean
make
```
