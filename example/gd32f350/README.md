# GD32F350 — Cortex-M4 + USB CDC

Board example for `config/cortex-m4`: USB FS **CDC** echo device, with an FPU-capable task (`TA_FPU`).

## Dependencies

Default firmware path (no env needed if present):

`third_party/gd32_f3x0_firmware/Firmware`

Override if needed:

```bash
export GD32_FW=<path to Firmware/ of https://github.com/DmitryTyazhlov/GD32F3x0_Firmware_Library_V2.3.0>
cd example/gd32f350
make clean
make
```

Toolchain: `arm-none-eabi-gcc`. The tree under `GD32_FW` supplies CMSIS, standard peripheral, and USBFS CDC class sources.

## What it does

- USB FS device (CDC) — host sees a virtual COM port
- IRQ → eventflag → USB task handles CDC RX/TX
- Float work in the USB task exercises M4 FPU context save/restore

No VFS/shell/lwIP — focused on the M4 port + USB class glue.

## Related

- CPU port: [`config/cortex-m4/`](../../config/cortex-m4/)
- Simpler M3 kernel smoke: [`../stm32l100/`](../stm32l100/)
