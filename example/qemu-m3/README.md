# QEMU Stellaris LM3S6965 — Cortex-M3 smoke for `config/cortex-m3`

No ST HAL / CMSIS-Device. Bundled CMSIS Core (`CMSIS/Core/Include`, Apache-2.0 from CMSIS_5 5.9.0).

## Build & run

```bash
cd example/qemu-m3
make
make run
```

Needs `arm-none-eabi-gcc` and `qemu-system-arm`.

Stop with **Ctrl+A X** (QEMU), or Ctrl+C.

## What boots

- kernel tasks / 1 ms SysTick
- UART0 console (`printf`)
- eventflag wait + soft IRQ via pending `GPIOA` → `iset_flg`
- ends with `qemu-m3 smoke ok` after a few flag/timeout cycles

## Related

- CPU port: [`config/cortex-m3/`](../../config/cortex-m3/)
- Board (needs CMSIS/HAL): [`../stm32l100/`](../stm32l100/)
- A53 QEMU: [`../qemu-a53/`](../qemu-a53/)
