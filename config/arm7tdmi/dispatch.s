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

        stmfd sp!, {r4 - r11,lr}   /* �쥸��������¸ */
        ldr   r0, =runtsk          /* runtsk���ɤ߹��� */
        ldr   r1, [r0]
        str   sp, [r1,#TCB_sp]      /* �����������å�����¸ */
        adr   r2, dispatch_r
        str   r2, [r1,#TCB_pc]      /* �¹ԺƳ����Ϥ���¸ */
        b     dispatcher_1

dispatch_r:
        ldmfd sp!,{r4 - r11,lr}
        /*
         * �������㳰�����롼����ε�ư
         * dispatch_r �� dispatcher_1 ����ƤӽФ���뤿�ᡤ
         * tcb�Υ��ɥ쥹��r1�����äƤ���
         */
        mov   pc,lr

dispatcher_1:
        /*
         *  ������������-��������-�+�--�T����-�����������
         */
        ldr   r0, =schedtsk   /* schedtsk 䪦��-���� */
        ldr   r1, [r0]
        ldr   r2, =runtsk     /* schedtsk � runtsk�T */
        str   r1, [r2]        /* schedtsk �������-����runtsk�NULL�T */
        ldr   sp, [r1,#TCB_sp] /* �������+���㹦� */
        ldr   pc, [r1,#TCB_pc] /* -T�L����L�+��㹦�   */


        CFI EndBlock cfiBlock0

        END

