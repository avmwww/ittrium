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
//        mov   r5, #(CPSR_SVC|CPSR_IRQ_BIT)  /* �����߶ػ�(�����ƥ�⡼��) */
//        mov   r4, #(CPSR_SVC)               /* �����ߵ���(�����ƥ�⡼��) */
        b     dispatcher_1

dispatch_r:
restore_contex:
        ; Restore contex
        ldmfd sp!,{r4 - r11,lr}
        /*
         * �������㳰�����롼����ε�ư
         * dispatch_r �� dispatcher_1 ����ƤӽФ���뤿�ᡤ
         * tcb�Υ��ɥ쥹��r1�����äƤ���
         */
//        ldrb  r0,[r1,#TCB_enatex]
//        tst   r0,#TCB_enatex_mask
//        beq   dispatch_r_1          /* enatex �� FALSE �ʤ�꥿���� */
//        ldr   r0,[r1,#TCB_texptn]   /* texptn�����               */
//        tst   r0,r0                 /* texptn ��0��̵�����         */
//        bne   call_texrtn           /* �������㳰�롼����θƤӽФ� */
//dispatch_r_1:   
        mov   pc,lr        
        
dispatcher_1:
        /*
         *  ������������-��������-�+�--�T����-�����������
         */
        ldr   r0, =schedtsk   /* schedtsk 䪦��-���� */
        ldr   r1, [r0]
        ldr   r2, =runtsk     /* schedtsk � runtsk�T */
        str   r1, [r2]        /* schedtsk �������-����runtsk�NULL�T */
//        cmp   r1, #0
//        beq   dispatcher_2        
//dispatcher_3:           
        ldr   sp, [r1,#TCB_sp] /* �������+���㹦� */
        ldr   pc, [r1,#TCB_pc] /* -T�L����L�+��㹦�   */
//dispatcher_2:
//        mov   r3,#1
//        str   r3, [r6]         /* interupt_count = 1 */
//        ldr   sp, =STACKTOP
//        /* sleep���-��䪬��-CPU���-���+����� */
//        mrs   r0, cpsr        /* FIQ䪬T-�           */
//        and   r0, r0, #CPSR_FIQ_BIT
//        orr   r0, r0, r4
//        msr   cpsr_cxsf, r0        /* �����-TL�+          */
//        WAIT_INTERRUPT
//        mrs   r0, cpsr        /* FIQ䪬T-�           */
//        and   r0, r0, #CPSR_FIQ_BIT
//        orr   r0, r0, r5
//        msr   cpsr_cxsf, r0        /* �����-�+�-          */        
//        mov   r3,#0
//        str   r3, [r6]        /* interrupt_count = 0 */   
//        b     dispatcher_1        



//BX       LR               ;; return

        END
