#include "ittrium.inc"

        NAME dispatch
        PUBLIC dispatch
        EXTERN runtsk
        EXTERN schedtsk
        EXTERN int_nesting

ARM_CONTROL_INT_DIS        EQU  0xC0                     ; Disable both FIQ and IRQ
ARM_CONTROL_FIQ_DIS        EQU  0x40                     ; Disable FIQ
ARM_CONTROL_IRQ_DIS        EQU  0x80                     ; Disable IRQ
ARM_CONTROL_THUMB          EQU  0x20                     ; Set THUMB mode
ARM_CONTROL_ARM            EQU  0x00                     ; Set ARM mode

ARM_MODE_MASK              EQU  0x1F
ARM_MODE_USR               EQU  0x10
ARM_MODE_FIQ               EQU  0x11
ARM_MODE_IRQ               EQU  0x12
ARM_MODE_SVC               EQU  0x13
ARM_MODE_ABT               EQU  0x17
ARM_MODE_UND               EQU  0x1B
ARM_MODE_SYS               EQU  0x1F

ARM_EXCEPT_RESET           EQU  0x00
ARM_EXCEPT_UNDEF_INSTR     EQU  0x01
ARM_EXCEPT_SWI             EQU  0x02
ARM_EXCEPT_PREFETCH_ABORT  EQU  0x03
ARM_EXCEPT_DATA_ABORT      EQU  0x04
ARM_EXCEPT_ADDR_ABORT      EQU  0x05
ARM_EXCEPT_IRQ             EQU  0x06
ARM_EXCEPT_FIQ             EQU  0x07

; Any of the registers R0 to R3, and R12, can be used as a scratch register by the function.
; The stack pointer register, R13/SP, must at all times point to or below the last
; element on the stack. In the eventuality of an interrupt, everything below the point
; the stack pointer points to, can be destroyed.
; The register R15/PC is dedicated for the Program Counter.
; The link register, R14/LR, holds the return address at the entrance of the function.

    RSEG CODE:CODE:NOROOT(2)
    CODE32

dispatch:
        ; Store return address, PC and LR
        STMFD   SP!, {LR}
        STMFD   SP!, {LR}
        ; Store registers
        STMFD   SP!, {R0-R12}
        ; Store CPSR
        MRS     R0, CPSR
        ; See if called from Thumb mode
        TST     LR, #1
        ; If yes, Set the T-bit
        ORRNE   R0, R0, #ARM_CONTROL_THUMB
        STMFD   SP!, {R0}
        ;
        LDR     R0, =runtsk
        LDR     R1, [R0]
        CMP     R1, #0
        BEQ     skip_save_context
        ; Store SP
        STR   SP, [R1,#TCB_sp]
skip_save_context:
        ; Move scheduled task to runnig state
        LDR   R0, =schedtsk
        LDR   R1, [R0]
        LDR   R2, =runtsk
        STR   R1, [R2]
        CMP   R1, #0
        BNE   restore_contex



        ; Return address is always dispatch_r
        adr   r2, dispatch_r
        str   r2, [r1,#TCB_pc]
        b     dispatcher_1
dispatch_r:


_interrupt_handler:

        LDR   R0, =int_nesting
        LDR   R1, [R0]
        CMP   R1, #0
        BNE   int_nest


        ; Save current task context
        PUSHM  R0,R1,R2,R3,A0,A1,SB,FB
        CMP.B #0, int_nesting
        JNZ int_nest
        MOV.W runtsk,   A1
        STC SP, TCB_sp:8[A1]
        ; Set up interrupt stack
        LDSTACK ISTACK, ISP
int_nest:
        INC.B int_nesting
        MOV.W _interrupt_vector, R0
        ; Enable interrupt
        FSET I
        JSR.A interrupt_handler
        ; Disable interrupt
        FCLR I
        DEC.B int_nesting
        JNZ int_nest_ret
        MOV.W runtsk,   A1
        ; Move scheduled task to runnig state
        MOV.W schedtsk, A0
        MOV.W A0, runtsk
        JZ _int_idle


restore_contex:
        ; Task's SP
        LDR   SP, [R1,#TCB_sp]
int_nest_ret:
        ; Pop new task's CPSR
        LDMFD   SP!, {R0}
        MSR     CPSR_cxsf, R0
        ; Restore new task's context
        LDMFD   SP!, {R0-R12, LR, PC}



        ldmfd sp!,{r4 - r11,lr}
        mov   pc,lr

dispatcher_1:
        /*
         *  ä¦ä¦ä¦ä¦å¬å¦å¦åðåòá-åãáæ¦ô¦¦ä-¦+¬--¿Tãä¦ä¦ä-äüä¦ä¦äùä¦ääáå
         */
        ldr   r0, =schedtsk   /* schedtsk äª¦ãä-¦¦äð */
        ldr   r1, [r0]
        ldr   r2, =runtsk     /* schedtsk äª runtskäT */
        str   r1, [r2]        /* schedtsk äìäää¦ää-ü¦÷ä¦runtskäªNULLäT */
        ldr   sp, [r1,#TCB_sp] /* å¬å¦åïå¦å¬å+åïäªã¹¦â */
        ldr   pc, [r1,#TCB_pc] /* -T¦L¦¦¦ëLã+¦äªã¹¦â   */

;********************************************************************************************************
; RESET EXCEPTION HANDLER
;********************************************************************************************************
arm_except_reset_handler:
        ; LR offset to return from this exception:  0
        ;  (there is no way to return from a RESET exception)
        ; Push working registers
        STMFD   SP!, {R0-R12, LR}
        ; Save link register
        MOV     R3, LR
        MOV     R0, #ARM_EXCEPT_RESET
        ; Branch to global exception handler
        B       arm_except_handler

;********************************************************************************************************
; UNDEFINED INSTRUCTION EXCEPTION HANDLER
;********************************************************************************************************
arm_except_undef_instr_handler:
        ; LR offset to return from this exception:  0
        STMFD   SP!, {R0-R12, LR}                                   ; Push working registers
        MOV     R3, LR                                              ; Save link register
        MOV     R0, #ARM_EXCEPT_UNDEF_INSTR
        ; Branch to global exception handler
        B       arm_except_handler

;********************************************************************************************************
; SOFTWARE INTERRUPT EXCEPTION HANDLER
;********************************************************************************************************
arm_except_swi_handler:
                                                                ; LR offset to return from this exception:  0
    STMFD   SP!, {R0-R12, LR}                                   ; Push working registers
    MOV     R3, LR                                              ; Save link register
    MOV     R0, #OS_CPU_ARM_EXCEPT_SWI                          ; Set exception ID to OS_CPU_ARM_EXCEPT_SWI
    B            OS_CPU_ARM_EXCEPT_HANDLER                      ; Branch to global exception handler


;********************************************************************************************************
;                                   PREFETCH ABORT EXCEPTION HANDLER
;********************************************************************************************************

OS_CPU_ARM_EXCEPT_PREFETCH_ABORT_HANDLER
    SUB     LR, LR, #4                                          ; LR offset to return from this exception: -4
    STMFD   SP!, {R0-R12, LR}                                   ; Push working registers
    MOV     R3, LR                                              ; Save link register
    MOV     R0, #OS_CPU_ARM_EXCEPT_PREFETCH_ABORT               ; Set exception ID to OS_CPU_ARM_EXCEPT_PREFETCH_ABORT
    B            OS_CPU_ARM_EXCEPT_HANDLER                      ; Branch to global exception handler


;********************************************************************************************************
;                                     DATA ABORT EXCEPTION HANDLER
;********************************************************************************************************

OS_CPU_ARM_EXCEPT_DATA_ABORT_HANDLER
    SUB     LR, LR, #8                                          ; LR offset to return from this exception: -8
    STMFD   SP!, {R0-R12, LR}                                   ; Push working registers
    MOV     R3, LR                                              ; Save link register
    MOV     R0, #OS_CPU_ARM_EXCEPT_DATA_ABORT                   ; Set exception ID to OS_CPU_ARM_EXCEPT_DATA_ABORT
    B            OS_CPU_ARM_EXCEPT_HANDLER                      ; Branch to global exception handler


;********************************************************************************************************
;                                    ADDRESS ABORT EXCEPTION HANDLER
;********************************************************************************************************

OS_CPU_ARM_EXCEPT_ADDR_ABORT_HANDLER
    SUB     LR, LR, #8                                          ; LR offset to return from this exception: -8
    STMFD   SP!, {R0-R12, LR}                                   ; Push working registers
    MOV     R3, LR                                              ; Save link register
    MOV     R0, #OS_CPU_ARM_EXCEPT_ADDR_ABORT                   ; Set exception ID to OS_CPU_ARM_EXCEPT_ADDR_ABORT
    B            OS_CPU_ARM_EXCEPT_HANDLER                      ; Branch to global exception handler


;********************************************************************************************************
;                                  INTERRUPT REQUEST EXCEPTION HANDLER
;********************************************************************************************************

OS_CPU_ARM_EXCEPT_IRQ_HANDLER
    SUB     LR, LR, #4                                          ; LR offset to return from this exception: -4
    STMFD   SP!, {R0-R12, LR}                                   ; Push working registers
    MOV     R3, LR                                              ; Save link register
    MOV     R0, #OS_CPU_ARM_EXCEPT_IRQ                          ; Set exception ID to OS_CPU_ARM_EXCEPT_IRQ
    B            OS_CPU_ARM_EXCEPT_HANDLER                      ; Branch to global exception handler


;********************************************************************************************************
;                               FAST INTERRUPT REQUEST EXCEPTION HANDLER
;********************************************************************************************************

OS_CPU_ARM_EXCEPT_FIQ_HANDLER
    SUB     LR, LR, #4                                          ; LR offset to return from this exception: -4
    STMFD   SP!, {R0-R12, LR}                                   ; Push working registers
    MOV     R3, LR                                              ; Save link register
    MOV     R0, #OS_CPU_ARM_EXCEPT_FIQ                          ; Set exception ID to OS_CPU_ARM_EXCEPT_FIQ
    B            OS_CPU_ARM_EXCEPT_HANDLER                      ; Branch to global exception handler


;********************************************************************************************************
;                                       GLOBAL EXCEPTION HANDLER
;********************************************************************************************************

OS_CPU_ARM_EXCEPT_HANDLER
    MRS     R1, SPSR                                            ; Save CPSR (i.e. exception's SPSR)

                                                                ; DETERMINE IF WE INTERRUPTED A TASK OR ANOTHER LOWER PRIORITY EXCEPTION
                                                                ;   SPSR.Mode = FIQ, IRQ, SVC, ABT, UND : Other exception
                                                                ;   SPSR.Mode = SYS                     : Task
                                                                ;   SPSR.Mode = USR                     : *unsupported state*
    AND     R2, R1, #OS_CPU_ARM_MODE_MASK
    CMP     R2,     #OS_CPU_ARM_MODE_SYS
    BNE     OS_CPU_ARM_EXCEPT_HANDLER_BreakExcept


;********************************************************************************************************
;                                  EXCEPTION HANDLER: TASK INTERRUPTED
;********************************************************************************************************

OS_CPU_ARM_EXCEPT_HANDLER_BreakTask
    MRS     R2, CPSR                                            ; Save exception's CPSR
    MOV     R4, SP                                              ; Save exception's stack pointer

                                                                ; Change to SYS mode & disable interruptions
    MSR     CPSR_c, #(OS_CPU_ARM_CONTROL_INT_DIS | OS_CPU_ARM_MODE_SYS)

                                                                ; SAVE TASK'S CONTEXT ONTO TASK'S STACK
    STMFD   SP!, {R3}                                           ;   Push task's PC
    STMFD   SP!, {LR}                                           ;   Push task's LR
    STMFD   SP!, {R5-R12}                                       ;   Push task's R12-R5
    LDMFD   R4!, {R5-R9}                                        ;   Move task's R4-R0 from exception stack to task's stack
    STMFD   SP!, {R5-R9}

    TST     R3, #1                                              ;   See if called from Thumb mode
    ORRNE   R1, R1, #OS_CPU_ARM_CONTROL_THUMB                   ;   If yes, Set the T-bit
    STMFD   SP!, {R1}                                           ;   Push task's CPSR (i.e. exception SPSR)

                                                                ; if (OSRunning == 1)
    LDR     R1, ?OS_Running
    LDRB    R1, [R1]
    CMP     R1, #1
    BNE     OS_CPU_ARM_EXCEPT_HANDLER_BreakTask_1

                                                                ; HANDLE NESTING COUNTER
    LDR     R3, ?OS_IntNesting                                  ;   OSIntNesting++;
    LDRB    R4, [R3]
    ADD     R4, R4, #1
    STRB    R4, [R3]

    LDR     R3, ?OS_TCBCur                                      ;   OSTCBCur->OSTCBStkPtr = SP;
    LDR     R4, [R3]
    STR     SP, [R4]

OS_CPU_ARM_EXCEPT_HANDLER_BreakTask_1
    MSR     CPSR_cxsf, R2                                       ; RESTORE INTERRUPTED MODE

    LDR     R1, ?OS_EXCEPT_HANDLER                              ; OS_EXCEPT_HANDLER();
    MOV     LR, PC
    BX      R1

                                                                ; Adjust exception stack pointer. This is needed because
                                                                ; exception stack is not used when restoring task context.
    ADD     SP, SP, #(14*4)

                                                                ; Change to SYS mode & disable interruptions.
    MSR     CPSR_c, #(OS_CPU_ARM_CONTROL_INT_DIS | OS_CPU_ARM_MODE_SYS)

                                                                ; Call OSIntExit(). This call MAY never return
                                                                ;  if a ready task with higher priority than
                                                                ;  the interrupted one is found.
    LDR     R0, ?OS_IntExit
    MOV     LR, PC
    BX      R0

                                                                ; RESTORE NEW TASK'S CONTEXT
    LDMFD   SP!, {R0}                                           ;    Pop new task's CPSR
    MSR     CPSR_cxsf, R0

    LDMFD   SP!, {R0-R12, LR, PC}                               ;    Pop new task's context


;********************************************************************************************************
;                               EXCEPTION HANDLER: EXCEPTION INTERRUPTED
;********************************************************************************************************

OS_CPU_ARM_EXCEPT_HANDLER_BreakExcept
    MRS     R2, CPSR                                            ; Save exception's CPSR

                                                                ; Change to SYS mode & disable interruptions
    MSR     CPSR_c, #(OS_CPU_ARM_CONTROL_INT_DIS | OS_CPU_ARM_MODE_SYS)

                                                                ; HANDLE NESTING COUNTER
    LDR     R3, ?OS_IntNesting                                  ;   OSIntNesting++;
    LDRB    R4, [R3]
    ADD     R4, R4, #1
    STRB    R4, [R3]

    MSR     CPSR_cxsf, R2                                       ; RESTORE INTERRUPTED MODE

    LDR     R3, ?OS_EXCEPT_HANDLER                              ; OS_EXCEPT_HANDLER();
    MOV     LR, PC
    BX      R3

                                                                ; Change to SYS mode & disable interruptions
    MSR     CPSR_c, #(OS_CPU_ARM_CONTROL_INT_DIS | OS_CPU_ARM_MODE_SYS)

                                                                ; HANDLE NESTING COUNTER
    LDR     R3, ?OS_IntNesting                                  ;   OSIntNesting--;
    LDRB    R4, [R3]
    SUB     R4, R4, #1
    STRB    R4, [R3]

    MSR     CPSR_cxsf, R2                                       ; RESTORE INTERRUPTED MODE

    LDMFD   SP!, {R0-R12, PC}^                                  ; Pull working registers and return from exception.


        END
