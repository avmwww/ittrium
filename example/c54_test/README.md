# TMS320 C54x test

Minimal bring-up stub for `config/tms320c54x` with the TI C54x tools (`cl500` / `lnk500` / `asm500`).

## Build

```bash
cd example/c54_test
# ensure cl500 / lnk500 / asm500 are on PATH (CCS C54x CGT)
make
```

Linker command file: `link.cmd`. Device revision flag in `Makefile.rules`: `-v548`.

## What it does

- Starts `init_tsk`, which loops on `dly_tsk(1000)`
- Timer install hooks are placeholders (`start_hw_timer` / `terminate_hw_timer`)

Prefer [`../c55x/`](../c55x/) for a fuller eventflag + software-IRQ demo on C55x.

## Related

- CPU port: [`config/tms320c54x/`](../../config/tms320c54x/)
