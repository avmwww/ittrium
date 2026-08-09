# c67px — `tms320c67px` smoke

Minimal µITRON smoke for `config/tms320c67px` (tasks + eventflag).

## Build

```bash
# export C6X_CGTOOLS=/path/to/ti-cgt-c6000_<ver>   # if cl6x is not on PATH
cd example/c67px
make clean && make
```

Artifact: `testc67px.out`.

Default `-mv6740` + `rts6740_elf.lib` (TI C6000 CGT 8.x). Historical silicon used `-mv67p` (CGT ≤7.4).
