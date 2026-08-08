#include "ittrium.inc"
; Any of the registers R0 to R3, and R12, can be used as a scratch register by the function.
; The stack pointer register, R13/SP, must at all times point to or below the last
; element on the stack. In the eventuality of an interrupt, everything below the point
; the stack pointer points to, can be destroyed.
; The register R15/PC is dedicated for the Program Counter.
; The link register, R14/LR, holds the return address at the entrance of the function.
        NAME dispatch
        PUBLIC dispatch
        PUBLIC dispatch_r
        EXTERN runtsk
        EXTERN schedtsk
        
        SECTION `.text`:CODE:NOROOT(2)
        ARM
dispatch:
        stmfd sp!, {r4 - r11,lr}
        ldr   r0, =runtsk
        ldr   r1, [r0]
        str   sp, [r1,#TCB_sp]
        ; Resume address after a later dispatch is always dispatch_r
        adr   r2, dispatch_r
        str   r2, [r1,#TCB_pc]
        b     dispatcher_1

dispatch_r:
restore_contex:
        ldmfd sp!,{r4 - r11,lr}
        mov   pc,lr
        
dispatcher_1:
        ldr   r0, =schedtsk
        ldr   r1, [r0]
        ldr   r2, =runtsk
        str   r1, [r2]
        ldr   sp, [r1,#TCB_sp]
        ldr   pc, [r1,#TCB_pc]

        END
