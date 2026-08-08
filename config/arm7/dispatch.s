#include "ittrium.inc"

        NAME dispatch
        PUBLIC dispatch
        PUBLIC dispatch_r
        EXTERN runtsk
        EXTERN schedtsk
        EXTERN int_nesting

ARM_CONTROL_THUMB          EQU  0x20

    RSEG CODE:CODE:NOROOT(2)
    CODE32

;
; Frame on task stack (make_task_context / restore_contex):
;   CPSR, R0-R12, LR, PC
;
dispatch:
        STMFD   SP!, {LR}
        STMFD   SP!, {LR}
        STMFD   SP!, {R0-R12}
        MRS     R0, CPSR
        TST     LR, #1
        ORRNE   R0, R0, #ARM_CONTROL_THUMB
        STMFD   SP!, {R0}

        LDR     R0, =runtsk
        LDR     R1, [R0]
        CMP     R1, #0
        BEQ     skip_save_context
        STR     SP, [R1,#TCB_sp]
skip_save_context:
        LDR     R0, =schedtsk
        LDR     R1, [R0]
        LDR     R2, =runtsk
        STR     R1, [R2]
        CMP     R1, #0
        BNE     restore_contex
        B       .

restore_contex:
        LDR     SP, [R1,#TCB_sp]
        LDMFD   SP!, {R0}
        MSR     CPSR_cxsf, R0
        LDMFD   SP!, {R0-R12, LR, PC}

; Kept for ABI compatibility with older call sites
dispatch_r:
        LDMFD   SP!, {R4-R11, LR}
        MOV     PC, LR

        END
