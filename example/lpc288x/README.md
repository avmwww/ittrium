# lpc288x — `config/lpc288x` smoke

Minimal µITRON smoke (init task + test task + eventflag).

## Toolchain

IAR Embedded Workbench for this CPU (`config/lpc288x` uses IAR asm/intrinsics).

Board register stubs (`T0*`, `INT_REQ5`) live in `kernel_config.h` for compile bring-up; replace with real LPC288x/SoC headers for hardware.

## Layout

- `include/kernel_config.h` — IDs, tick, board ticker macros
- `src/src/test.c` — `cre_flg` / `cre_tsk` / `dly_tsk` / `set_flg`
- Kernel: `kernel/ittrium.c` + `config/lpc288x/cpu_support.*`

Create an EW project (or `Makefile.iar`) that compiles those sources and links a board startup/CRT.
