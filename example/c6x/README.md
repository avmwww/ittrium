# c6x — `tms320c6x` smoke

Minimal µITRON smoke for `config/tms320c6x` (tasks + eventflag).

## Build

```bash
# export C6X_CGTOOLS=/path/to/ti-cgt-c6000_<ver>   # if cl6x is not on PATH
cd example/c6x
make clean && make
```

Artifact: `testc6x.out`.

Default `-mv6740` + `rts6740_elf.lib` (TI C6000 CGT 8.x). Historical silicon was C6700 (`DEVREV=6700`, CGT ≤7.4).
