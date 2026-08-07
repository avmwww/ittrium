# Kria / ZynqMP — Cortex-A53 AArch64 EL1 (no MMU)

ittrium board port for the **kria_rtx** carrier (GEM3 + UART1).  
Does **not** use the Xilinx no-OS BSP in EL3+MMU mode — register reference only.

## Build

```bash
export PATH=/opt/xilinx/Vitis/2023.2/gnu/aarch64/lin/aarch64-none/bin:$PATH
cd example/kria-rtx
make
```

Artifact: `ittrium-kria-rtx.elf` (load address `0x00100000`).

## Hardware map

| Block | Phys | IRQ |
|-------|------|-----|
| UART1 (Cadence) | `0xFF010000` | 54 |
| GEM3 | `0xFF0E0000` | 95 |
| GIC Dist / CPU | `0xF9010000` / `0xF9020000` | Group1 (EL1 NS) |
| PHY | Marvell 88E1512 @ MDIO 0, RGMII-ID | RST MIO59 (FSBL) |

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
- kria_rtx no-OS is a register/PHY reference only — do not copy its EL3+MMU bring-up
- Stock KV260 / KR260 boards often differ in PHY and MIO — check the device tree; do not copy blindly
