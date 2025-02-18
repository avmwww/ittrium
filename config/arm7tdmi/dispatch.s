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

        stmfd sp!, {r4 - r11,lr}   /* ¥ì¥¸¥¹¥¿¤ÎÊÝÂ¸ */
        ldr   r0, =runtsk          /* runtsk¤òÆÉ¤ß¹þ¤à */
        ldr   r1, [r0]
        str   sp, [r1,#TCB_sp]      /* ¥¿¥¹¥¯¥¹¥¿¥Ã¥¯¤òÊÝÂ¸ */
        adr   r2, dispatch_r
        str   r2, [r1,#TCB_pc]      /* ¼Â¹ÔºÆ³«ÈÖÃÏ¤òÊÝÂ¸ */
        b     dispatcher_1

dispatch_r:
        ldmfd sp!,{r4 - r11,lr}
        /*
         * ¥¿¥¹¥¯Îã³°½èÍý¥ë¡¼¥Á¥ó¤Îµ¯Æ°
         * dispatch_r ¤Ï dispatcher_1 ¤«¤é¸Æ¤Ó½Ð¤µ¤ì¤ë¤¿¤á¡¤
         * tcb¤Î¥¢¥É¥ì¥¹¤Ïr1¤ËÆþ¤Ã¤Æ¤¤¤ë
         */
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


        CFI EndBlock cfiBlock0

        END

