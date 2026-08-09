# c67px — `tms320c67px` smoke

Minimal µITRON smoke for `config/tms320c67px` (tasks + eventflag).

## Build

```bash
# export C6X_CGTOOLS=$HOME/ti/ccs_avm/ti-cgt-c6000_8.1.2   # if needed
cd example/c67px
make clean && make
```

Artifact: `testc67px.out`.

Default `-mv6740` + `rts6740_elf.lib` (TI C6000 CGT 8.x). Historical silicon used `-mv67p` (CGT ≤7.4).
