# TMS320 C55x example

Kernel smoke for `config/tms320c55x` using the TI C55x codegen tools (`cl55` / `lnk55` / `masm55`).

Typical target: CCS C5500 toolchain (e.g. `c5500_4.4.1`) with `cl55` on `PATH`.

## Build

```bash
cd example/c55x
make clean
make
```

Artifact: `test55x.out` (linker command file `lnkx.cmd`).

## What it does

- `init_tsk` creates an eventflag + `test_tsk`, installs software interrupt `#30`
- Every ~2.5 s (scaled by `SIMUL_DIV`) fires `INTR #30`; the ISR does `iset_flg`
- `test_tsk` waits on the flag (`twai_flg`) and prints ready/timeout

`SIMUL_DIV` in `src/src/test.c` stretches delays for the CCS simulator (`100`) vs hardware (`1`).

## Related

- CPU port: [`config/tms320c55x/`](../../config/tms320c55x/)
- Older C54x stub: [`../c54_test/`](../c54_test/)
