#include "ittrium.inc"
; Any of the registers R0 to R3, and R12, can be used as a scratch register by the function.
; The stack pointer register, R13/SP, must at all times point to or below the last
; element on the stack. In the eventuality of an interrupt, everything below the point
; the stack pointer points to, can be destroyed.
; The register R15/PC is dedicated for the Program Counter.
; The link register, R14/LR, holds the return address at the entrance of the function.
        NAME dispatch
        PUBLIC dispatch
        EXTERN runtsk
        EXTERN schedtsk
        
        SECTION `.text`:CODE:NOROOT(2)
        ARM
dispatch:
        ; Store registers R4..R11, R14 to stack
        stmfd sp!, {r4 - r11,lr}
        ldr   r0, =runtsk
        ldr   r1, [r0]
        ; Store SP
        str   sp, [r1,#TCB_sp]
        ; Return address is always dispatch_r
        adr   r2, dispatch_r
        str   r2, [r1,#TCB_pc]
//        ldr   r6, =interrupt_count  /* r6 <-interrupt_count */
//        mov   r5, #(CPSR_SVC|CPSR_IRQ_BIT)  /* 割り込み禁止(システムモード) */
//        mov   r4, #(CPSR_SVC)               /* 割り込み許可(システムモード) */
        b     dispatcher_1

dispatch_r:
restore_contex:
        ; Restore contex
        ldmfd sp!,{r4 - r11,lr}
        /*
         * タスク例外処理ルーチンの起動
         * dispatch_r は dispatcher_1 から呼び出されるため，
         * tcbのアドレスはr1に入っている
         */
//        ldrb  r0,[r1,#TCB_enatex]
//        tst   r0,#TCB_enatex_mask
//        beq   dispatch_r_1          /* enatex が FALSE ならリターン */
//        ldr   r0,[r1,#TCB_texptn]   /* texptnをロード               */
//        tst   r0,r0                 /* texptn が0で無ければ         */
//        bne   call_texrtn           /* タスク例外ルーチンの呼び出し */
//dispatch_r_1:   
        mov   pc,lr        
        
dispatcher_1:
        /*
         *  筅筅筅筅絳絋絋辮纈�-繝瘋�熙��-�+�--�T邃�筅�-糶筅筅糯筅粐痳
         */
        ldr   r0, =schedtsk   /* schedtsk 筧�邃-Ζ粽 */
        ldr   r1, [r0]
        ldr   r2, =runtsk     /* schedtsk 筧 runtsk�T */
        str   r1, [r2]        /* schedtsk 粲粐筅粐-���筅runtsk筧NULL�T */
//        cmp   r1, #0
//        beq   dispatcher_2        
//dispatcher_3:           
        ldr   sp, [r1,#TCB_sp] /* 絳絋繽絋絳�+繽筧禮�� */
        ldr   pc, [r1,#TCB_pc] /* -T�LΖ��L�+�筧禮��   */
//dispatcher_2:
//        mov   r3,#1
//        str   r3, [r6]         /* interupt_count = 1 */
//        ldr   sp, =STACKTOP
//        /* sleep纈�-繝筧�や-CPU筅糯-娯�+�粫糴 */
//        mrs   r0, cpsr        /* FIQ筧�T-�           */
//        and   r0, r0, #CPSR_FIQ_BIT
//        orr   r0, r0, r4
//        msr   cpsr_cxsf, r0        /* �熙��-TL�+          */
//        WAIT_INTERRUPT
//        mrs   r0, cpsr        /* FIQ筧�T-�           */
//        and   r0, r0, #CPSR_FIQ_BIT
//        orr   r0, r0, r5
//        msr   cpsr_cxsf, r0        /* �熙��-�+�-          */        
//        mov   r3,#0
//        str   r3, [r6]        /* interrupt_count = 0 */   
//        b     dispatcher_1        



//BX       LR               ;; return

        END
