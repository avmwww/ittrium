        .include ittrium.ha

; A15 used as frame page pointer (FP)
FP	.set	A15
; B14 used as data page pointer (DP)
; Remark: DP is global and points to begin of .bss section
DP	.set	B14
; B15 used as stack pointer (SP)
SP	.set	B15

          .asg 1, reg_addr_offt
REG_ADDR  .macro  x
x         .set reg_addr_offt*4
          .eval reg_addr_offt+1, reg_addr_offt
          .endm


; Floating-point multiplier configuration register
              REG_ADDR   _FMCR
; Floating-point auxiliary configuration register
              REG_ADDR   _FAUCR
; Floating-point adder configuration register
              REG_ADDR   _FADCR
; Addressing mode register
              REG_ADDR   _AMR
; Interrupt return pointer
              REG_ADDR   _IRP
; Registers File A
              REG_ADDR   _A31
              REG_ADDR   _A30
              REG_ADDR   _A29
              REG_ADDR   _A28
              REG_ADDR   _A27
              REG_ADDR   _A26
              REG_ADDR   _A25
              REG_ADDR   _A24
              REG_ADDR   _A23
              REG_ADDR   _A22
              REG_ADDR   _A21
              REG_ADDR   _A20
              REG_ADDR   _A19
              REG_ADDR   _A18
              REG_ADDR   _A17
              REG_ADDR   _A16
			  REG_ADDR   _A15
              REG_ADDR   _A14
              REG_ADDR   _A13
              REG_ADDR   _A12
              REG_ADDR   _A11
              REG_ADDR   _A10
              REG_ADDR   _A9
              REG_ADDR   _A8
              REG_ADDR   _A7
              REG_ADDR   _A6
              REG_ADDR   _A5
              REG_ADDR   _A4
              REG_ADDR   _A3
              REG_ADDR   _A2
              REG_ADDR   _A1
              REG_ADDR   _A0
; Registers File B
              REG_ADDR   _B31
              REG_ADDR   _B30
              REG_ADDR   _B29
              REG_ADDR   _B28
              REG_ADDR   _B27
              REG_ADDR   _B26
              REG_ADDR   _B25
              REG_ADDR   _B24
              REG_ADDR   _B23
              REG_ADDR   _B22
              REG_ADDR   _B21
              REG_ADDR   _B20
              REG_ADDR   _B19
              REG_ADDR   _B18
              REG_ADDR   _B17
              REG_ADDR   _B16
			  REG_ADDR   _B14
              REG_ADDR   _B13
              REG_ADDR   _B12
              REG_ADDR   _B11
              REG_ADDR   _B10
              REG_ADDR   _B9
              REG_ADDR   _B8
              REG_ADDR   _B7
              REG_ADDR   _B6
              REG_ADDR   _B5
              REG_ADDR   _B4
              REG_ADDR   _B3
              REG_ADDR   _B2
              REG_ADDR   _B1
; Control status register
              REG_ADDR   _CSR
; First saves
              REG_ADDR   _B0
;---------------------------
              REG_ADDR   context_len
;*******************************************************************************
; 16 interrupts
;*******************************************************************************
        .global __int_vector_table
INT_STACK_LEN   .set 512
        .global __int_stack
        .bss __int_stack, INT_STACK_LEN, 4
        .text
;*******************************************************************************
; B3 containt address of interrupt routine
;*******************************************************************************
	.align	32
        .newblock
        .global __interrupt_handler
__interrupt_handler:
        ; B registers
        STW     .D2T2    B1,*SP--
        STW     .D2T2    B2,*SP--
        STW     .D2T2    B3,*SP--
        STW     .D2T2    B4,*SP--
        STW     .D2T2    B5,*SP--
        STW     .D2T2    B6,*SP--
        STW     .D2T2    B7,*SP--
        STW     .D2T2    B8,*SP--
        STW     .D2T2    B9,*SP--
        STW     .D2T2   B10,*SP--
        STW     .D2T2   B11,*SP--
        STW     .D2T2   B12,*SP--
        STW     .D2T2   B13,*SP--
        STW     .D2T2   B14,*SP--
		; 67plus B registers
		STW     .D2T2   B16,*SP--
        STW     .D2T2   B17,*SP--
        STW     .D2T2   B18,*SP--
        STW     .D2T2   B19,*SP--
        STW     .D2T2   B20,*SP--
        STW     .D2T2   B21,*SP--
        STW     .D2T2   B22,*SP--
        STW     .D2T2   B23,*SP--
        STW     .D2T2   B24,*SP--
        STW     .D2T2   B25,*SP--
        STW     .D2T2   B26,*SP--
        STW     .D2T2   B27,*SP--
        STW     .D2T2   B28,*SP--
        STW     .D2T2   B29,*SP--
        STW     .D2T2   B30,*SP--
        STW     .D2T2   B31,*SP--
        ; A registers
        STW     .D2T1    A0,*SP--
        STW     .D2T1    A1,*SP--
        STW     .D2T1    A2,*SP--
        STW     .D2T1    A3,*SP--
        STW     .D2T1    A4,*SP--
        STW     .D2T1    A5,*SP--
        STW     .D2T1    A6,*SP--
        STW     .D2T1    A7,*SP--
        STW     .D2T1    A8,*SP--
        STW     .D2T1    A9,*SP--
        STW     .D2T1   A10,*SP--
        STW     .D2T1   A11,*SP--
        STW     .D2T1   A12,*SP--
        STW     .D2T1   A13,*SP--
        STW     .D2T1   A14,*SP--
        STW     .D2T1   A15,*SP--
		; 67plus A registers
		STW     .D2T1   A16,*SP--
        STW     .D2T1   A17,*SP--
        STW     .D2T1   A18,*SP--
        STW     .D2T1   A19,*SP--
        STW     .D2T1   A20,*SP--
        STW     .D2T1   A21,*SP--
        STW     .D2T1   A22,*SP--
        STW     .D2T1   A23,*SP--
        STW     .D2T1   A24,*SP--
        STW     .D2T1   A25,*SP--
        STW     .D2T1   A26,*SP--
        STW     .D2T1   A27,*SP--
        STW     .D2T1   A28,*SP--
        STW     .D2T1   A29,*SP--
        STW     .D2T1   A30,*SP--
        STW     .D2T1   A31,*SP--
        || MVC      IRP,B1
        ; The return address IRP
        STW     .D2T2   B1,*SP--
        ; Addressing Mode Register (AMR)
        || MVC      AMR,B1
        STW     .D2T2   B1,*SP--
        ; Floating-Point Adder Configuration Register (FADCR)
        || MVC      FADCR,B1
        STW     .D2T2   B1,*SP--
        ; Floating-Point Auxiliary Configuration Register (FAUCR)
        || MVC      FAUCR,B1
        STW     .D2T2   B1,*SP--
        ; Floating-Point Multiplier Configuration Register (FMCR)
        || MVC      FMCR,B1
        STW     .D2T2   B1,*SP
        ;
        LDW     .D2T2   *+DP(_runtsk),B1
        NOP     4
        ; SP
  [B1]  STW     .D2T2   SP,*+B1(TCB_sp)
        ; New stack pointer
        MVKL   (__int_stack + INT_STACK_LEN - 4)&(~7), SP
        MVKH   (__int_stack + INT_STACK_LEN - 4)&(~7), SP
        ;
        ; Call interrupt handler
        MVKL    .S2     __int_vector_table, B1
        MVKH    .S2     __int_vector_table, B1
        LDW     .D2T2   *+B1[B0], B0
        MVK     .S2     0x0,B1
        NOP      3
  [B0]  B       .S2     B0
        MVKL    .S2     restore_context,B3
        MVKH    .S2     restore_context,B3
        STW     .D2T2   B1,*+DP(_runtsk)
        NOP      2
restore_context:
        ; POP registers from new TCB 
        LDW     .D2T2   *+DP(_schedtsk),B1
        NOP      4
  [!B1] B        __idle_task
        STW     .D2T2   B1,*+DP(_runtsk)
        NOP      4
        ; SP
        LDW     .D2T2   *+B1(TCB_sp), B15
        NOP      4
        ; ¬осстанавливаем специальные регистры
        ; FMCR
        LDW     .D2T2   *SP++,B4
        ; FAUCR
        LDW     .D2T2   *SP++,B3
        ; FADCR
        LDW     .D2T2   *SP++,B2
        ; AMR
        LDW     .D2T2   *SP++,B1
        ; The return address IRP
        LDW     .D2T2   *SP++,B0
        ; A registers
        LDW     .D2T1   *SP++,A31
        ||  MVC .S2     B4,FMCR
        LDW     .D2T1   *SP++,A30
        ||  MVC .S2     B3,FAUCR
        LDW     .D2T1   *SP++,A29
        ||  MVC .S2     B2,FADCR
        LDW     .D2T1   *SP++,A28
        ||  MVC .S2     B1,AMR
        LDW     .D2T1   *SP++,A27
        || MVC          B0,IRP
        LDW     .D2T1   *SP++,A26
        LDW     .D2T1   *SP++,A25
        LDW     .D2T1   *SP++,A24
        LDW     .D2T1   *SP++,A23
        LDW     .D2T1   *SP++,A22
        LDW     .D2T1   *SP++,A21
        LDW     .D2T1   *SP++,A20
        LDW     .D2T1   *SP++,A19
        LDW     .D2T1   *SP++,A18
        LDW     .D2T1   *SP++,A17
        LDW     .D2T1   *SP++,A16
		LDW     .D2T1   *SP++,A15
        LDW     .D2T1   *SP++,A14
        LDW     .D2T1   *SP++,A13
        LDW     .D2T1   *SP++,A12
        LDW     .D2T1   *SP++,A11
        LDW     .D2T1   *SP++,A10
        LDW     .D2T1   *SP++,A9
        LDW     .D2T1   *SP++,A8
        LDW     .D2T1   *SP++,A7
        LDW     .D2T1   *SP++,A6
		LDW     .D2T1   *SP++,A5
        LDW     .D2T1   *SP++,A4
        LDW     .D2T1   *SP++,A3
        LDW     .D2T1   *SP++,A2
        LDW     .D2T1   *SP++,A1
        LDW     .D2T1   *SP++,A0
        ; B registers
        LDW     .D2T2   *SP++,B31
        LDW     .D2T2   *SP++,B30
        LDW     .D2T2   *SP++,B29
        LDW     .D2T2   *SP++,B28
        LDW     .D2T2   *SP++,B27
        LDW     .D2T2   *SP++,B26
        LDW     .D2T2   *SP++,B25
        LDW     .D2T2   *SP++,B24
        LDW     .D2T2   *SP++,B23
        LDW     .D2T2   *SP++,B22
		LDW     .D2T2   *SP++,B21
		LDW     .D2T2   *SP++,B20
        LDW     .D2T2   *SP++,B19
        LDW     .D2T2   *SP++,B18
        LDW     .D2T2   *SP++,B17
        LDW     .D2T2   *SP++,B16
        LDW     .D2T2   *SP++,B14
        LDW     .D2T2   *SP++,B13
        LDW     .D2T2   *SP++,B12
        LDW     .D2T2   *SP++,B11
        LDW     .D2T2   *SP++,B10
        LDW     .D2T2   *SP++,B9
        LDW     .D2T2   *SP++,B8
        LDW     .D2T2   *SP++,B7
        LDW     .D2T2   *SP++,B6
        LDW     .D2T2   *SP++,B5
        ; CSR
        LDW     .D2T2   *+SP(4*4),B0  ;?????
        LDW     .D2T2   *SP++,B4
        LDW     .D2T2   *SP++,B3
        LDW     .D2T2   *SP++,B2
        LDW     .D2T2   *SP++,B1
        ; CSR
        LDW     .D2T2   *++SP,B0
        || MVC  .S2     B0,CSR
        B       .S2     IRP
        NOP      5
;*******************************************************************************
;*******************************************************************************
        .global __idle_task
__idle_task:
        ; New stack pointer
        MVKL   __int_stack + INT_STACK_LEN - 4, SP
        MVKH   __int_stack + INT_STACK_LEN - 4, SP
        ; Enable interrupts
        MVC     .S2     CSR,B4
        OR      .S2     1,B4,B1
        MVC     .S2     B1,CSR
idle_loop?:
        IDLE
        NOP     5
        B       idle_loop?
        NOP     5

;*******************************************************************************
; void dispatch(void);
;*******************************************************************************
        .global _dispatch
_dispatch:
        ; Disable interrupts
        MVC     .S2     CSR,B4
        AND     .S2     ~1,B4,B1
        MVC     .S2     B1,CSR
        LDW     .D2T2   *+DP(_schedtsk),B2
        LDW     .D2T2   *+DP(_runtsk),B1
        NOP     4
        CMPEQ  .L2    B1, B2, B0
 [B0]   B      .S1    dispatch_exit?
        NOP     5
 [!B1]  B      .S1    restore_context
        NOP     5
;-------------------------------------------------------------------------------
; Save context
;-------------------------------------------------------------------------------
        STW     .D2T2   B0,*SP--
        AND     .S2     ~1,B4,B4
        STW     .D2T2   B4,*SP--  ;CSR
        ; B registers
        STW     .D2T2    B1,*SP--
        STW     .D2T2    B2,*SP--
        STW     .D2T2    B3,*SP--
        STW     .D2T2    B4,*SP--
        STW     .D2T2    B5,*SP--
        STW     .D2T2    B6,*SP--
        STW     .D2T2    B7,*SP--
        STW     .D2T2    B8,*SP--
        STW     .D2T2    B9,*SP--
        STW     .D2T2   B10,*SP--
        STW     .D2T2   B11,*SP--
        STW     .D2T2   B12,*SP--
        STW     .D2T2   B13,*SP--
        STW     .D2T2   B14,*SP--
		;67Plus registers
		STW     .D2T2   B16,*SP--
		STW     .D2T2   B17,*SP--
		STW     .D2T2   B18,*SP--
		STW     .D2T2   B19,*SP--
		STW     .D2T2   B20,*SP--
		STW     .D2T2   B21,*SP--
		STW     .D2T2   B22,*SP--
		STW     .D2T2   B23,*SP--
		STW     .D2T2   B24,*SP--
		STW     .D2T2   B25,*SP--
		STW     .D2T2   B26,*SP--
		STW     .D2T2   B27,*SP--
		STW     .D2T2   B28,*SP--
		STW     .D2T2   B29,*SP--
		STW     .D2T2   B30,*SP--
		STW     .D2T2   B31,*SP--
        ; A registers
        STW     .D2T1    A0,*SP--
        STW     .D2T1    A1,*SP--
        STW     .D2T1    A2,*SP--
        STW     .D2T1    A3,*SP--
        STW     .D2T1    A4,*SP--
        STW     .D2T1    A5,*SP--
        STW     .D2T1    A6,*SP--
        STW     .D2T1    A7,*SP--
        STW     .D2T1    A8,*SP--
        STW     .D2T1    A9,*SP--
        STW     .D2T1   A10,*SP--
        STW     .D2T1   A11,*SP--
        STW     .D2T1   A12,*SP--
        STW     .D2T1   A13,*SP--
        STW     .D2T1   A14,*SP--
        STW     .D2T1   A15,*SP--
		;67Plus registers
		STW     .D2T1   A16,*SP--
        STW     .D2T1   A17,*SP--
        STW     .D2T1   A18,*SP--
        STW     .D2T1   A19,*SP--
        STW     .D2T1   A20,*SP--
        STW     .D2T1   A21,*SP--
        STW     .D2T1   A22,*SP--
        STW     .D2T1   A23,*SP--
        STW     .D2T1   A24,*SP--
        STW     .D2T1   A25,*SP--
        STW     .D2T1   A26,*SP--
        STW     .D2T1   A27,*SP--
        STW     .D2T1   A28,*SP--
        STW     .D2T1   A29,*SP--
        STW     .D2T1   A30,*SP--
        STW     .D2T1   A31,*SP--
        ; The return address IRP
        STW     .D2T2   B3,*SP--
        ; Addressing Mode Register (AMR)
        || MVC      AMR,B1
        STW     .D2T2   B1,*SP--
        ; Floating-Point Adder Configuration Register (FADCR)
        || MVC      FADCR,B1
        STW     .D2T2   B1,*SP--
        ; Floating-Point Auxiliary Configuration Register (FAUCR)
        || MVC      FAUCR,B1
        STW     .D2T2   B1,*SP--
        ; Floating-Point Multiplier Configuration Register (FMCR)
        || MVC      FMCR,B1
        STW     .D2T2   B1,*SP
        ;
        LDW     .D2T2   *+DP(_runtsk),B1
        NOP     4
        ; SP
  [B1]  STW     .D2T2   SP,*+B1(TCB_sp)
        B       .S1     restore_context
        NOP      5
dispatch_exit?:
        B       .S2     B3
        ; Restore GIE
        MVC     .S2     B4,CSR
		NOP     4
;*******************************************************************************
; void make_task_context(TCB *tcb)
;                         A4
;*******************************************************************************
        .newblock
        .global _make_task_context
_make_task_context:
        LDW     .D1T2  *+A4(TCB_stk),B1
        LDW     .D1T2  *+A4(TCB_task),B2
        LDW     .D1T2  *+A4(TCB_exinf),B4
        MVK     .S2     0x00B0,B0
        NOP
        ADDK    .S2    (-4),B1
        AND     .S2    ~7,B1,B1
;-------------------------------------------------------------------------------
; Save context
;-------------------------------------------------------------------------------
        STW     .D2T2   B0,*B1--
        ; Control Status Register (CSR), PGIE|GIE
        || MVK  .S2     0x0003,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00B1,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00B2,B0
        STW     .D2T2   B0,*B1--
        ; task
        STW     .D2T2   B2,*B1--
        || MVK  .S2     0x00B4,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00B5,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00B6,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00B7,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00B8,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00B9,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B10,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B11,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B12,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B13,B0
        STW     .D2T2   B0,*B1--
        ; DP
        STW     .D2T2   DP,*B1--
        || MVK  .S2     0x0B16,B0
        ; 67plus registers
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B17,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B18,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B19,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B20,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B21,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B22,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B23,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B24,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B25,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B26,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B27,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B28,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B29,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B30,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0B31,B0
		;
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00A0,B0
		STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00A1,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00A2,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00A3,B0
        STW     .D2T2   B0,*B1--
        ; exinf
        STW     .D2T2   B4,*B1--
        || MVK  .S2     0x00A5,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00A6,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00A7,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00A8,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x00A9,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0A10,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0A11,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0A12,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0A13,B0
        STW     .D2T2   B0,*B1--
        || MVK  .S2     0x0A14,B0
        STW     .D2T2   B0,*B1--
        ; FP
        STW     .D2T2   B1,*B1--
		|| MVK  .S2     0x0A16,B0
		; 67plus registers
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A17,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A18,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A19,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A20,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A21,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A22,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A23,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A24,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A25,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A26,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A27,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A28,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A29,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A30,B0
		STW     .D2T2   B0,*B1--
		|| MVK  .S2     0x0A31,B0
		STW		.D2T2	B0,*B1--
		|| MVK  .S2     0x0000,B0
        ; The return address IRP
        STW     .D2T2   B2,*B1--
        ; ќпределяем специальные регистры
        ; Addressing Mode Register (AMR)
        STW     .D2T2   B0,*B1--
        ; Floating-Point Adder Configuration Register (FADCR)
        STW     .D2T2   B0,*B1--
        ; Floating-Point Auxiliary Configuration Register (FAUCR)
        STW     .D2T2   B0,*B1--
        ; Floating-Point Multiplier Configuration Register (FMCR)
        STW     .D2T2   B0,*B1
        B       .S2     B3
        ; SP
        STW        B1,*+A4(TCB_sp)
        NOP      4

;*******************************************************************************
; Interrupts vector table
;*******************************************************************************
INT_VECTOR  .macro   vec_no
        STW     .D2T2   B0,*SP--
        || MVC  .S2     CSR,B0
        B        __interrupt_handler
        STW     .D2T2   B0,*SP--
        MVK     .S2     vec_no,B0
        NOP      3
            .endm
;-------------------------------------------------------------------------------
        .global __int_vectors
        .sect ".vectors"
;-------------------------------------------------------------------------------
	.align	32
__int_vectors:
IVEC_INT_00:
        INT_VECTOR  0x00
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_01:
        INT_VECTOR  0x01
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_02:
        INT_VECTOR  0x02
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_03:
		INT_VECTOR  0x03
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_04:
        INT_VECTOR  0x04
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_05:
        INT_VECTOR  0x05
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_06:
        INT_VECTOR  0x06
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_07:
        INT_VECTOR  0x07
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_08:
        INT_VECTOR  0x08
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_09:
        INT_VECTOR  0x09
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_10:
        INT_VECTOR  0x0A
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_11:
        INT_VECTOR  0x0B
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_12:
        INT_VECTOR  0x0C
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_13:
        INT_VECTOR  0x0D
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_14:
        INT_VECTOR  0x0E
;-------------------------------------------------------------------------------
	.align	32
IVEC_INT_15:
        INT_VECTOR  0x0F
;-------------------------------------------------------------------------------


        .end
