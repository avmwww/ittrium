#include "ittrium.inc"

        NAME dispatch

        PUBLIC dispatch
        EXTERN runtsk
        EXTERN schedtsk

        CFI Names cfiNames0
        CFI StackFrame CFA R13 DATA
        CFI Resource R0:32, R1:32, R2:32, R3:32, R4:32, R5:32, R6:32, R7:32
        CFI Resource R8:32, R9:32, R10:32, R11:32, R12:32, R13:32, R14:32
        CFI EndNames cfiNames0

        CFI Common cfiCommon0 Using cfiNames0
        CFI CodeAlign 4
        CFI DataAlign 4
        CFI ReturnAddress R14 CODE
        CFI CFA R13+0
        CFI R0 Undefined
        CFI R1 Undefined
        CFI R2 Undefined
        CFI R3 Undefined
        CFI R4 SameValue
        CFI R5 SameValue
        CFI R6 SameValue
        CFI R7 SameValue
        CFI R8 SameValue
        CFI R9 SameValue
        CFI R10 SameValue
        CFI R11 SameValue
        CFI R12 Undefined
        CFI R14 SameValue
        CFI EndCommon cfiCommon0

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function dispatch
        ARM
dispatch:

        stmfd sp!, {r4 - r11,lr}   /* レジスタの保存 */
        ldr   r0, =runtsk          /* runtskを読み込む */
        ldr   r1, [r0]
        str   sp, [r1,#TCB_sp]      /* タスクスタックを保存 */
        adr   r2, dispatch_r
        str   r2, [r1,#TCB_pc]      /* 実行再開番地を保存 */
        b     dispatcher_1

dispatch_r:
        ldmfd sp!,{r4 - r11,lr}
        /*
         * タスク例外処理ルーチンの起動
         * dispatch_r は dispatcher_1 から呼び出されるため，
         * tcbのアドレスはr1に入っている
         */
        mov   pc,lr

dispatcher_1:
        /*
         *  筅筅筅筅絳絋絋辮纈�-繝瘋�熙��-�+�--�T邃�筅�-糶筅筅糯筅粐痳
         */
        ldr   r0, =schedtsk   /* schedtsk 筧�邃-Ζ粽 */
        ldr   r1, [r0]
        ldr   r2, =runtsk     /* schedtsk 筧 runtsk�T */
        str   r1, [r2]        /* schedtsk 粲粐筅粐-���筅runtsk筧NULL�T */
        ldr   sp, [r1,#TCB_sp] /* 絳絋繽絋絳�+繽筧禮�� */
        ldr   pc, [r1,#TCB_pc] /* -T�LΖ��L�+�筧禮��   */


        CFI EndBlock cfiBlock0

        END

