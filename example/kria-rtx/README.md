# Kria / ZynqMP — Cortex-A53 AArch64 EL1 (no MMU)

Порт ittrium под carrier **kria_rtx** (GEM3 + UART1). Не использует BSP EL3+MMU из no-OS.

## Build

```bash
export PATH=/opt/xilinx/Vitis/2023.2/gnu/aarch64/lin/aarch64-none/bin:$PATH
cd example/kria-rtx
make
```

Артефакт: `ittrium-kria-rtx.elf` (load `0x00100000`).

## Hardware map

| Block | Phys | IRQ |
|-------|------|-----|
| UART1 (Cadence) | `0xFF010000` | 54 |
| GEM3 | `0xFF0E0000` | 95 |
| GIC Dist / CPU | `0xF9010000` / `0xF9020000` | Group1 (EL1 NS) |
| PHY | Marvell 88E1512 @ MDIO 0, RGMII-ID | RST MIO59 (FSBL) |

## Boot assumptions

- FSBL/`psu_init` уже поднял clocks/MIO/PHY reset
- Образ стартует в **EL1** (или boot.S сам спускается с EL3/EL2)
- **MMU/caches off** — как QEMU-порт
- CNTPCT: `_low_level_init` включает IOU SCNTRS при необходимости

## Bring-up smoke

1. UART1 115200 8N1 → shell
2. `ps` / `irq` / `load`
3. `ifconfig` → `gem3` / `192.168.1.50` (правьте IP в `test.c`)
4. `ping <gw>` / TCP echo `:7`

## Notes

- Верх тот же: `CFG_USE_*` + `console` / `netdev` / lwIP
- kria_rtx no-OS — только референс регистров UART/GEM/PHY
- Stock KV260/KR260: другие PHY/MIO — сверять DT, не копировать слепо
