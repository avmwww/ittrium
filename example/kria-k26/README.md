# Kria K26 SOM — Cortex-A53 AArch64 EL1 (no MMU)

ittrium board port for the **Kria K26 SOM** (Zynq UltraScale+ MPSoC): Cadence UART1 + GEM3.  
EL1, no MMU — same model as `example/qemu-a53`. Does not use a Xilinx no-OS BSP in EL3+MMU mode.

Carrier boards differ (PHY, MIO, Ethernet wiring). Defaults below match a typical K26 carrier with GEM3 RGMII; adjust `target.h` / PHY setup for your board.

## Build

```bash
cd example/kria-k26
make
```

Requires `aarch64-none-elf-gcc` on `PATH` (or `CROSS_COMPILE=…`).  
Artifact: `ittrium-kria-k26.elf` (load address `0x00100000`).

## Hardware map (PS)

| Block | Phys | IRQ |
|-------|------|-----|
| UART1 (Cadence) | `0xFF010000` | 54 |
| GEM3 | `0xFF0E0000` | 95 |
| GIC Dist / CPU | `0xF9010000` / `0xF9020000` | Group1 (EL1 NS) |
| PHY (carrier) | often Marvell @ MDIO 0, RGMII | reset via carrier GPIO/MIO |

## Boot assumptions

- FSBL / `psu_init` has already configured clocks, MIO, and PHY reset
- Image starts in **EL1** (or `boot.S` drops from EL3/EL2)
- **MMU and caches off** — same model as the QEMU port
- CNTPCT: `_low_level_init` enables IOU SCNTRS if firmware left it off

## Bring-up smoke

1. UART1 115200 8N1 → shell
2. `ps` / `irq` / `load`
3. `ifconfig` → `gem3` / `192.168.1.50` (change IP in `test.c` for your LAN)
4. `ping <gw>` / TCP echo `:7`

## Notes

- Same upper stack: `CFG_USE_*` + `console` / `netdev` / lwIP
- Stock KV260 / KR260 / custom carriers may differ in PHY and MIO — check the device tree
