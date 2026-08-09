# m16c62 — `config/m16c62` smoke

Minimal µITRON smoke (init task + test task + eventflag).

## Toolchain

IAR Embedded Workbench for this CPU (`config/m16c62` uses IAR asm/intrinsics).

Needs IAR M16C. Provide real `ENABLE_TICKER_INT` / `TICKER_VEC_NO` for hardware timer.

## Layout

- `include/kernel_config.h` — IDs, tick, board ticker macros
- `src/src/test.c` — `cre_flg` / `cre_tsk` / `dly_tsk` / `set_flg`
- Kernel: `kernel/ittrium.c` + `config/m16c62/cpu_support.*`

Create an EW project (or `Makefile.iar`) that compiles those sources and links a board startup/CRT.
