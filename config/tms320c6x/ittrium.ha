INTERRUPT_STACK_LEN   .set 1024
; TCB structure, offsets   queue(prev, next); 
TCB_sp  .set      8

TCB_task   .set    12
TCB_stk    .set    16
TCB_stksz  .set    20
TCB_exinf  .set    28

        .global _runtsk
        .global _schedtsk

        .global _main

        .ref cinit
        .ref ___bss__
        .ref ___pinit__
        .ref ___end__


