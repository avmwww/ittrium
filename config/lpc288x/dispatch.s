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
//        mov   r5, #(CPSR_SVC|CPSR_IRQ_BIT)  /* ³ä¤ê¹þ¤ß¶Ø»ß(¥·¥¹¥Æ¥à¥â¡¼¥É) */
//        mov   r4, #(CPSR_SVC)               /* ³ä¤ê¹þ¤ßµö²Ä(¥·¥¹¥Æ¥à¥â¡¼¥É) */
        b     dispatcher_1

dispatch_r:
restore_contex:
        ; Restore contex
        ldmfd sp!,{r4 - r11,lr}
        /*
         * ¥¿¥¹¥¯Îã³°½èÍý¥ë¡¼¥Á¥ó¤Îµ¯Æ°
         * dispatch_r ¤Ï dispatcher_1 ¤«¤é¸Æ¤Ó½Ð¤µ¤ì¤ë¤¿¤á¡¤
         * tcb¤Î¥¢¥É¥ì¥¹¤Ïr1¤ËÆþ¤Ã¤Æ¤¤¤ë
         */
//        ldrb  r0,[r1,#TCB_enatex]
//        tst   r0,#TCB_enatex_mask
//        beq   dispatch_r_1          /* enatex ¤¬ FALSE ¤Ê¤é¥ê¥¿¡¼¥ó */
//        ldr   r0,[r1,#TCB_texptn]   /* texptn¤ò¥í¡¼¥É               */
//        tst   r0,r0                 /* texptn ¤¬0¤ÇÌµ¤±¤ì¤Ð         */
//        bne   call_texrtn           /* ¥¿¥¹¥¯Îã³°¥ë¡¼¥Á¥ó¤Î¸Æ¤Ó½Ð¤· */
//dispatch_r_1:   
        mov   pc,lr        
        
dispatcher_1:
        /*
         *  ä¦ä¦ä¦ä¦å¬å¦å¦åðåòá-åãáæ¦ô¦¦ä-¦+¬--¿Tãä¦ä¦ä-äüä¦ä¦äùä¦ääáå
         */
        ldr   r0, =schedtsk   /* schedtsk äª¦ãä-¦¦äð */
        ldr   r1, [r0]
        ldr   r2, =runtsk     /* schedtsk äª runtskäT */
        str   r1, [r2]        /* schedtsk äìäää¦ää-ü¦÷ä¦runtskäªNULLäT */
//        cmp   r1, #0
//        beq   dispatcher_2        
//dispatcher_3:           
        ldr   sp, [r1,#TCB_sp] /* å¬å¦åïå¦å¬å+åïäªã¹¦â */
        ldr   pc, [r1,#TCB_pc] /* -T¦L¦¦¦ëLã+¦äªã¹¦â   */
//dispatcher_2:
//        mov   r3,#1
//        str   r3, [r6]         /* interupt_count = 1 */
//        ldr   sp, =STACKTOP
//        /* sleepåòá-åãäª¬¤ä-CPUä¦äù-¸äí+¦äèäû */
//        mrs   r0, cpsr        /* FIQäª¬T-¦           */
//        and   r0, r0, #CPSR_FIQ_BIT
//        orr   r0, r0, r4
//        msr   cpsr_cxsf, r0        /* ¦ô¦¦ä-TLä+          */
//        WAIT_INTERRUPT
//        mrs   r0, cpsr        /* FIQäª¬T-¦           */
//        and   r0, r0, #CPSR_FIQ_BIT
//        orr   r0, r0, r5
//        msr   cpsr_cxsf, r0        /* ¦ô¦¦ä-¦+¬-          */        
//        mov   r3,#0
//        str   r3, [r6]        /* interrupt_count = 0 */   
//        b     dispatcher_1        



//BX       LR               ;; return

        END
