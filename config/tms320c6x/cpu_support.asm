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
        ;AND   ~7,SP,SP
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
        LDW     .D2T1   *SP++,A15
        ||  MVC .S2     B4,FMCR
        LDW     .D2T1   *SP++,A14
        ||  MVC .S2     B3,FAUCR
        LDW     .D2T1   *SP++,A13
        ||  MVC .S2     B2,FADCR
        LDW     .D2T1   *SP++,A12
        ||  MVC .S2     B1,AMR
        LDW     .D2T1   *SP++,A11
        || MVC          B0,IRP
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
        LDW     .D2T2   *+SP(4*4),B0
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
        STW     .D2T2   B4,*SP--
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
        || MVK  .S2     0x00A0,B0
        ;
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
        || MVK  .S2     0x0000,B0
        ; FP
        STW     .D2T2   B1,*B1--
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

        ; –езервируем место под context
        ADDK    .S2     (4 -_context_len),SP
        ; ¬озврат из подпрограммы по B3
        STW     .D2T2   B3,*+SP(_B3)
        ; —охраняем специальные регистры
        ||  MVC         AMR, B0
        STW     .D2T2   B0,*+SP(_AMR)
        ||  MVC         CSR,B0
        STW     .D2T2   B0,*+SP(_CSR)
;        ||  MVC         IER,B0
;        STW     .D2T2   B0,*+SP(_IER)
        ||  MVC         PCE1,B0
        STW     .D2T2   B0,*+SP(_PCE1)
        ||  MVC         FADCR,B0
        STW     .D2T2   B0,*+SP(_FADCR)
        ||  MVC         FAUCR,B0
        STW     .D2T2   B0,*+SP(_FAUCR)
        ||  MVC         FMCR,B0
        STW     .D2T2   B0,*+SP(_FMCR)
        ; —охраняем регистры
        STW     .D2T1   A0,*+SP(_A0)  ; 
        ||  MV  .L1X    SP,A0                   ; A0 = SP  
        STW     .D2T1   A1,*+SP(_A1)  ; 
;        ||  addk     .s1    (_IRP),A0     ; Correct SP for POP of TCB
        STW     .D2T1   A0,*+SP(_B15) ;  Save correct SP
        ;
        STW     .D2T1   A2, *+SP(_A2)
        STW     .D2T1   A3, *+SP(_A3)
        STW     .D2T1   A4, *+SP(_A4)   
        STW     .D2T1   A5, *+SP(_A5) 
        STW     .D2T1   A6, *+SP(_A6) 
        STW     .D2T1   A7, *+SP(_A7) 
        STW     .D2T1   A8, *+SP(_A8)
        STW     .D2T1   A9, *+SP(_A9)
        STW     .D2T1   A10,*+SP(_A10)
        STW     .D2T1   A11,*+SP(_A11)
        STW     .D2T1   A12,*+SP(_A12) 
        STW     .D2T1   A13,*+SP(_A13) 
        STW     .D2T1   A14,*+SP(_A14) 
        STW     .D2T1   A15,*+SP(_A15) 

        LDW     .D2T2   *+DP(_OSTCBCur),B0     ; B0 = OSTCBCur
        STW     .D2T2   B1,*+SP(_B1)  ;  
        STW     .D2T2   B2,*+SP(_B2)  ;  
        STW     .D2T2   B3,*+SP(_B3)  ;  Save for cosmetical reasons
        STW     .D2T2   B4,*+SP(_B4)  ;  
        STW     .D2T2   B5,*+SP(_B5)  ;  
        STW     .D2T2   B6,*+SP(_B6)  ;  


        STW             SP,*B0                  ; previous OSTCBCur->OSTCBStkPtr = SP ;
    STW     .D2T2   B7 ,*+SP(_B7 ) ;  
    STW     .D2T2   B8 ,*+SP(_B8 ) ;   
    STW     .D2T2   B9 ,*+SP(_B9 ) ;   
    STW     .D2T2   B10,*+SP(_B10)  ;  
    STW     .D2T2   B11,*+SP(_B11)  ;  
    STW     .D2T2   B12,*+SP(_B12)  ;  
    STW     .D2T2   B13,*+SP(_B13)  ;  
    STW     .D2T2   B14,*+SP(_B14)  ;  
;   STW     .D2T2   B15,*+SP(_B15)  ;  already saved by first statement

        ; Save current task stack pointers
        MOV AC1, XAR0
        MOV XSP, dbl(*AR0(#TCB_sp))
        MOV XSSP, dbl(*AR0(#TCB_ssp))
skip_save_context:
        ; Move scheduled task to runnig state
        MOV AC0, dbl(*(#_runtsk))
        BCC __int_idle, AC0==#0
        MOV AC0, XAR0
        ; Restore task stack pointers
        MOV dbl(*AR0(#TCB_sp)), XSP
        MOV dbl(*AR0(#TCB_ssp)), XSSP
        B __restote_context



    LDW     .D2T2   *+DP(_OSTCBCur),B0     ; B0 = OSTCBCur
    STW     .D2T2   B1,*+SP(_B1)  ;  
    STW     .D2T2   B2,*+SP(_B2)  ;  
    STW     .D2T2   B3,*+SP(_B3)  ;  Save for cosmetical reasons
    STW     .D2T2   B4,*+SP(_B4)  ;  
    STW     .D2T2   B5,*+SP(_B5)  ;  
    STW     .D2T2   B6,*+SP(_B6)  ;  








    addk     .s2            (4 -_Framesize),SP   ; Push one Frame

    STW     .D2T2   B3,*+SP(_StartAddress)  ;  b3 start address
    
    STW     .D2T2   B0,*+SP(_B0)  ;  
||  mvc      AMR,B0
    STW     .D2T2   B0,*+SP(_AMR)  ;  
||  mvc      CSR,B0
    STW     .D2T2   B0,*+SP(_CSR)  ;  
||  mvc      IER,B0
    STW     .D2T2   B0,*+SP(_IER)  ;  


    
    STW     .D2T1   A0,*+SP(_A0)  ; 
||  MV      .L1X    SP,A0                   ; A0 = SP  
    STW     .D2T1   A1,*+SP(_A1)  ; 
||  addk     .s1    (_IRP),A0     ; Correct SP for POP of TCB
    STW     .D2T1   A0,*+SP(_B15) ;  Save correct SP


    STW     .D2T1   A2,*+SP(_A2)  ; 
    STW     .D2T1   A3,*+SP(_A3)  ; 
    STW     .D2T1   A4,*+SP(_A4)  ;    
    STW     .D2T1   A5,*+SP(_A5)  ;  
    STW     .D2T1   A6,*+SP(_A6)  ;  
    STW     .D2T1   A7,*+SP(_A7)  ;  
    STW     .D2T1   A8,*+SP(_A8)  ; 
    STW     .D2T1   A9,*+SP(_A9)  ; 
    STW     .D2T1   A10,*+SP(_A10) ; 
    STW     .D2T1   A11,*+SP(_A11) ; 
    STW     .D2T1   A12,*+SP(_A12)  ; 
    STW     .D2T1   A13,*+SP(_A13)  ; 
    STW     .D2T1   A14,*+SP(_A14)  ; 
    STW     .D2T1   A15,*+SP(_A15)  ; 

    LDW     .D2T2   *+DP(_OSTCBCur),B0     ; B0 = OSTCBCur
    STW     .D2T2   B1,*+SP(_B1)  ;  
    STW     .D2T2   B2,*+SP(_B2)  ;  
    STW     .D2T2   B3,*+SP(_B3)  ;  Save for cosmetical reasons
    STW     .D2T2   B4,*+SP(_B4)  ;  
    STW     .D2T2   B5,*+SP(_B5)  ;  
    STW     .D2T2   B6,*+SP(_B6)  ;  

;----------------------------------------------------------------;
;  Save the current task's stack pointer into the current task's ;
;  OS TCB                                                        ;
;----------------------------------------------------------------;
    STW             SP,*B0                  ; previous OSTCBCur->OSTCBStkPtr = SP ;
    STW     .D2T2   B7 ,*+SP(_B7 ) ;  
    STW     .D2T2   B8 ,*+SP(_B8 ) ;   
    STW     .D2T2   B9 ,*+SP(_B9 ) ;   
    STW     .D2T2   B10,*+SP(_B10)  ;  
    STW     .D2T2   B11,*+SP(_B11)  ;  
    STW     .D2T2   B12,*+SP(_B12)  ;  
    STW     .D2T2   B13,*+SP(_B13)  ;  
    STW     .D2T2   B14,*+SP(_B14)  ;  
;   STW     .D2T2   B15,*+SP(_B15)  ;  already saved by first statement






;----------------------------------------------------------------;
;  Save the current task's stack pointer into the current task's ;
;  OS TCB                                                        ;
;----------------------------------------------------------------;

;
;  TAKES 40 cycles to execute
;  240 nano sec
;
    ADDK    .S2     (4 -_Framesize),SP      ; Push one Frame
    STW     .D2T2   B3,*+SP(_StartAddress)  ;  B3 start address
    STW     .D2T2   B3,*+SP(_B3)            ;  Save for cosmetical reasons


||  MVC      AMR,B0
    STW     .D2T2   B0,*+SP(_AMR)  ;  
||  MVC      CSR,B0
    STW     .D2T2   B0,*+SP(_CSR)  ;  
||  MVC      IER,B0
    STW     .D2T2   B0,*+SP(_IER)  ;  



    STW        A3,*+SP(_A3)  ; 
    LDW       *+DP(_OSTCBCur),B0     ; B0 = OSTCBCur

;-------------------------------------------;
;  Get the address of the TCB that is going ;
;  to execute                               ;
;-------------------------------------------;
    LDW     .D2T2   *+DP(_OSTCBHighRdy),B4 ; B4 = OSTCBHighRdy

    STW     .D2T1   A10,*+SP(_A10) ; 
    STW     .D2T1   A11,*+SP(_A11) ; 
    STW     .D2T1   A12,*+SP(_A12)  ; 
    STW     .D2T1   A13,*+SP(_A13)  ; 
    STW     .D2T1   A14,*+SP(_A14)  ; 
    STW     .D2T1   A15,*+SP(_A15)  ; 

;-------------------------------;
;     Set it as the current TCB ;
; OSTCBCur = OSTCBHighRdy
;-------------------------------;
    STW     .D2T2   B4,*+DP(_OSTCBCur)      

    STW             SP,*B0                  ; previous OSTCBCur->OSTCBStkPtr = SP ;

    STW     .D2T2   B10,*+SP(_B10)  ;  
||  MV              SP,B10                   ; B10 = SP  
    STW     .D2T2   B11,*+SP(_B11)  ;  
||  addk           (_IRP),B10       ;         Correct SP for POP of TCB
;--------------------------------------------------;
;   In the TCB is the taskіs stack pointer value   ;
;   Get it                                         ;
;--------------------------------------------------;
; SP = OSTCBHighRdy->OSTCBStkPtr    
    LDW     .D2T2   *B4,SP                 

    STW             B10,*+SP(_B15)  ;         Save correct SP
    STW     .D2T2   B12,*+SP(_B12)  ;  
    STW     .D2T2   B13,*+SP(_B13)  ;  
    STW     .D2T2   B14,*+SP(_B14)  ;  

; POP registers from new TCB 

    LDW     .D2T2   *+SP(_StartAddress),B3  ;  b3 start address

    LDW     .D2T1   *+SP(_A3),A3  ; 

    LDW     .D2T1   *+SP(_A10),A10 ; 
    LDW     .D2T1   *+SP(_A11),A11 ; 
    LDW     .D2T1   *+SP(_A12),A12  ; 
    LDW     .D2T1   *+SP(_A13),A13  ; 
    LDW     .D2T1   *+SP(_A14),A14  ; 
    LDW     .D2T1   *+SP(_A15),A15  ; 
    
    LDW     .D2T2   *+SP(_B10),B10  ;  
    LDW     .D2T2   *+SP(_B11),B11  ;  
    LDW     .D2T2   *+SP(_B12),B12  ;  
    LDW     .D2T2   *+SP(_B14),B14  ;  
    LDW     .D2T2   *+SP(_B13),B13  ;  
    LDW     .D2T2   *+SP(_B3),B3    ;  
    LDW     .D2T2   *+SP(_B15),B15  ;  
||  B       .S2     b3              ;  
	nop     5



;******************************************************************************
;* FUNCTION NAME: _i2c_handler                                                *
;*                                                                            *
;*   Regs Modified     : A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,B0,B1,B2,B3,B4,B5,B6,  *
;*                           B7,B8,B9,SP                                      *
;*   Regs Used         : A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,B0,B1,B2,B3,B4,B5,B6,  *
;*                           B7,B8,B9,DP,SP                                   *
;*   Local Frame Size  : 0 Args + 4 Auto + 84 Save = 88 byte                  *
;******************************************************************************
_i2c_handler:
;** --------------------------------------------------------------------------*
             STW     .D2T2   B9,*SP--(88)      ; |84| 
             STW     .D2T2   B7,*+SP(84)       ; |84| 
             STW     .D2T2   B6,*+SP(80)       ; |84| 
             STW     .D2T2   B5,*+SP(76)       ; |84| 
             STW     .D2T2   B4,*+SP(72)       ; |84| 
             STW     .D2T2   B3,*+SP(68)       ; |84| 
             STW     .D2T2   B2,*+SP(64)       ; |84| 
             STW     .D2T2   B1,*+SP(60)       ; |84| 
             STW     .D2T2   B0,*+SP(56)       ; |84| 
             STW     .D2T2   B8,*+SP(52)       ; |84| 
             STW     .D2T1   A9,*+SP(48)       ; |84| 
             STW     .D2T1   A7,*+SP(44)       ; |84| 
             STW     .D2T1   A6,*+SP(40)       ; |84| 
             STW     .D2T1   A5,*+SP(36)       ; |84| 
             STW     .D2T1   A4,*+SP(32)       ; |84| 
             STW     .D2T1   A3,*+SP(28)       ; |84| 
             STW     .D2T1   A2,*+SP(24)       ; |84| 
             STW     .D2T1   A1,*+SP(20)       ; |84| 
             STW     .D2T1   A0,*+SP(16)       ; |84| 
             STW     .D2T1   A8,*+SP(12)       ; |84| 
             NOP             2

;** --------------------------------------------------------------------------*
           LDDW    .D2T2   *+SP(72),B5:B4    ; |151| 
           LDDW    .D2T2   *+SP(64),B3:B2    ; |151| 
           LDDW    .D2T2   *+SP(56),B1:B0    ; |151| 
           LDW     .D2T2   *+SP(52),B8       ; |151| 
           LDW     .D2T1   *+SP(48),A9       ; |151| 
           LDDW    .D2T1   *+SP(40),A7:A6    ; |151| 
           LDDW    .D2T1   *+SP(32),A5:A4    ; |151| 
           LDDW    .D2T1   *+SP(24),A3:A2    ; |151| 
           LDDW    .D2T1   *+SP(16),A1:A0    ; |151| 
           LDW     .D2T1   *+SP(12),A8       ; |151| 

           RET     .S2     IRP               ; |151| 
||         LDDW    .D2T2   *+SP(80),B7:B6    ; |151| 

           LDW     .D2T2   *++SP(88),B9      ; |151| 
           NOP             4
           ; BRANCH OCCURS                   ; |151| 


*****************************************************************************
*
* component   :     
*                   TMS320C6x Real Time Operating System
*
* module name :     ucos6201.ASM
*
* subcomponent:
*
******************************************************************************
*
* function    :                                                   
*                 Contains code for the scheduling.
*                 The code has to be in assembler as it is interfaced with the
*                 UC/OS code.
* description :                                                   
*
******************************************************************************
*
*
* dependencies: 
*               TMS320C6201 DSP 
*               TI compiler version 3 with the following switches
*               TMS320C6x C Compiler Shell       Version 3.00
*				
*  cl6x -b  -pdv -g -k -ss -s -q -mw -mi50 -o3 -on2 -os -x2  -al 
*       -frc_obj -fsc_asm asmfiles/*.asm software\ucos-II\tms6201\OS_CPU_a.asm 
*		*.c -z link.cmd -l rts6201.lib -o product/kern2.out
*
*		One context switch (130 MHz) takes 550 nano sec.
*		One context switch (130 MHz) by interrupt takes 1 microsec
*		Thus the normal 8000 context switches per second is
*		at maximum 8 millisec, 0.8 % cpu load.
*		Not bad at all!!!
*
* author      : Kenneth Blake - 
* history     : 
*
*
******************************************************************************/



*****************************************************************
* 
*                        ucos6201.ASM
*                   Real Time Operating System
*****************************************************************



FP	.set	A15
DP	.set	B14
SP	.set	B15


_StartAddress .set 0+4
_A0  .set  4        +4
_A1  .set  8        +4
_A2  .set  12       +4
_A3  .set  16       +4
_A4  .set  20       +4
_A5  .set  24       +4
_A6  .set  28       +4
_A7  .set  32       +4
_A8  .set  36       +4
_A9  .set  40       +4
_A10 .set  44       +4
_A11 .set  48       +4
_A12 .set  52       +4
_A13 .set  56       +4
_A14 .set  60       +4
_A15 .set  64       +4
_B0  .set  68       +4
_B1  .set  72       +4
_B2  .set  76       +4
_B3  .set  80       +4
_B4  .set  84       +4
_B5  .set  88       +4
_B6  .set  92       +4
_B7  .set  96       +4
_B8  .set  100      +4
_B9  .set  104      +4
_B10 .set  108      +4
_B11 .set  112      +4
_B12 .set  116      +4
_B13 .set  120      +4
_B14 .set  124      +4
_B15 .set  128      +4
_AMR .set  132      +4
_CSR .set  136      +4
_IER .set  140      +4
_IRP .set  144      +4
_Framesize .set 148 +4













DelaySlot_for_Branch  .set 5

	.global	$bss
          .global  __vector  ; /* Memory location for default   */

           .global _TimerZeroTick
           .global _Always_Enabled_Interrupts

          .global _OSStartHighRdy
          .global _OSTCBHighRdy
          .global _OSTCBCur
		  .global _OSPrioCur
		  .global _OSPrioHighRdy
          .global _OSCtxSw
          .global _OSIntCtxSw

          .global _c6201_Restor
          .global _c6201_Save
          .global _C6201_Register_Initialisation

		   .global _OSTaskSwHook
		   .global 	_ReturnCurrentDP
		   .global _OSRunning


	.align	32
	.global	_ReturnCurrentDP
;----------------------------------------------------------------------
; 140 | int ReturnCurrentDP( void )                                            
;----------------------------------------------------------------------

;******************************************************************************
;* FUNCTION NAME: _ReturnCurrentDP                                            *
;*                                                                            *
;*   Regs Modified     : A4                                                   *
;*   Regs Used         : A4,B3                                                *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
_ReturnCurrentDP:
;** --------------------------------------------------------------------------*
;----------------------------------------------------------------------
;----------------------------------------------------------------------
            mv   DP,a4
;** --------------------------------------------------------------------------*
           B       .S2     B3           ; |144| 
           NOP             5
           ; BRANCH OCCURS ; |144| 




;----------------------------------------------;
;    Initialise the processors registers with  ;
;    defined values                            ;
;----------------------------------------------;
	.align	32

_C6201_Register_Initialisation

        MVK     .S2     0x0,B0
b
        mvc   B0,AMR
        mvc   B0,ISR
        mvc   B0,ICR
        mvc   B0,IER
        mvc   B0,IRP
        mvc   B0,ICR
        mvc   B0,NRP
;        MVK     .S2     0x100,B0 big endian
        mvc   B0,CSR

        mvkl  .s2 __vector,B0
        mvkh  .s2 __vector,B0
        mvc   B0,ISTP


        MVK     .S1     0x0A0,A0
        MVK     .S1     0x0A1,A1
        MVK     .S1     0x0A2,A2
        MVK     .S1     0x0A3,A3
        MVK     .S1     0x0A4,A4
        MVK     .S1     0x0A5,A5
        MVK     .S1     0x0A6,A6
        MVK     .S1     0x0A7,A7
        MVK     .S1     0x0A8,A8
        MVK     .S1     0x0A9,A9
        MVK     .S1     0x0A10,A10
        MVK     .S1     0x0A11,A11
        MVK     .S1     0x0A12,A12
        MVK     .S1     0x0A13,A13
        MVK     .S1     0x0A14,A14
;        MVK     .S1     0x0,A15  ; Frame pointer

        MVK     .S2     0x0B0,B0
        MVK     .S2     0x0B1,B1
        MVK     .S2     0x0B2,B2
;        MVK     .S2    0x0B3,B3  Return address
        MVK     .S2     0x0B4,B4
        MVK     .S2     0x0B5,B5
        MVK     .S2     0x0B6,B6
        MVK     .S2     0x0B7,B7
        MVK     .S2     0x0B8,B8
        MVK     .S2     0x0B9,B9
        MVK     .S2     0x0B10,B10
        MVK     .S2     0x0B11,B11
        MVK     .S2     0x0B12,B12
        MVK     .S2     0x0B13,B13
;        MVK     .S2    0xA140,B14  ; data pointer
;        MVK     .S2     0x0,B15  ; Stack pointer

        B       .S2     B3           ; |133| 
        NOP             5


    
	.align	32
_c6201_Save  
*
* Save registers in INITIAL_REGISTER_FRAME  format
* Also save the stack pointer in the TCB
* for potential interrupt context switch
*   IRP contains the return address for the interrupt handler
*   B3 the return address in the interrupt handler
*   On the stack is the saved B3
*   RUNS IN   D I S A B L E D   M O D E
;   SP--> Free    0
;         xxxxx   4
;         User B3 8
; B3 contains return address in interrupt handler         
; IRP contains the address of the interrupted environment
;   The environments B# value is on the stack
;   SP--> User B3
;         

    addk     .s2            (12 -_Framesize),SP   ; Push one Frame
    
;   SP--> Free
;         Frame start
;         .....
;         ......
;         .....
;         .....
;         Frame IRP Including User B3
;         
;         

; DISABLED INTERRUPT MODE 

;--------------------------------------------------------------;
;    Save the Return address pointing within the interrupt handler 
;    It is in register B3, written by the caller
;--------------------------------------------------------------;

;---------------------------;
;    Save the registers     ;
;---------------------------;

    STW     .D2T2   B0,*+SP(_B0)  ;         ; Save B0
||  mvc      AMR,B0                         ; B0 = AMR
    STW     .D2T2   B1,*+SP(_B1)  ;         ; Save B1
||  mvc      CSR,B1                         ; B1 = CSR
    STW     .D2T2   B2,*+SP(_B2)  ;         ; Save B2
||  mvc      IER,B2				            ; B2 =Interrupt enable register

    LDW     .D2T2   *+DP(_Always_Enabled_Interrupts),B0 ; |92| 
    STW     .D2T2  B0,*+SP(_AMR)  ;        ; Save AMR
    STW     .D2T2   B1,*+SP(_CSR)  ;        ; Save CSR
    STW     .D2T2   B2,*+SP(_IER)  ;        ; Save IER
    
    
;---------------------------------------------;
;     Set the Global Interrupt Enable GIE and ;
;     Previous Global Interrupt Enable PGIE   ;
;     in CSR                                  ;
;---------------------------------------------;
    
    
    STW     .D2T2   B4,*+SP(_B4)  ;         ; Save B4
||  mvc      IRP,B4				            ; The return address in the interrupted environment
    STW     .D2T2   B4,*+SP (_StartAddress)    ;  Save return address in interrupt handler start address
||  or       3,B1,B1                        ; Set GIE
||  MVC     .S2     B0,IER                  ; IER = Always_Enabled_Interrupts

    
    STW     .D2T2   B5,*+SP(_B5)  ;  
||  mvc     b1,CSR                          ;Enable Always_Enabled_Interrupts

;---------------------------------------------------;
;    From here on the always enabled interrupts may ;
;    interrupt the processing                       ;
;---------------------------------------------------;


    STW     .D2T2   B6,*+SP(_B6)  ;  
    STW     .D2T2   B7 ,*+SP(_B7 ) ;  
    STW     .D2T2   B8 ,*+SP(_B8 ) ;   
    STW     .D2T2   B9 ,*+SP(_B9 ) ;   
    STW     .D2T2   B10,*+SP(_B10)  ;  
    STW     .D2T2   B11,*+SP(_B11)  ;  
    STW     .D2T2   B12,*+SP(_B12)  ;  
    STW     .D2T2   B13,*+SP(_B13)  ;  
    STW     .D2T2   B14,*+SP(_B14)  ;  


    STW     .D2T1   A0,*+SP(_A0)     ; Save A0 
;----------------------------------------------------------;
;     Save the interrupted environments B3 Register contents
;     It is currently at the IRP position in the frame
;     Get it into A0
;----------------------------------------------------------;
    LDW           *+SP    (_IRP),A0  ; A0 = The interrupted environments B3
    
    STW     .D2T1   A1,*+SP(_A1)     ; Save A1
    STW     .D2T1   A2,*+SP(_A2)     ; Save A2
    STW     .D2T1   A3,*+SP(_A3)     ; Save A3
    STW     .D2T1   A4,*+SP(_A4)     ; Save A4    
    
;------------------------------------------------------;
;     Save the Environments B3 contents which is in A0 ;
;------------------------------------------------------;
    STW             A0,*+SP(_B3)     ; Save B3  <------- B3  B3 B3 
    STW     .D2T1   A5,*+SP(_A5)  ;  
    STW     .D2T1   A6,*+SP(_A6)  ;  
    STW     .D2T1   A7,*+SP(_A7)  ;  
    STW     .D2T1   A8,*+SP(_A8)  ; 
    STW     .D2T1   A9,*+SP(_A9)  ; 

||  MV      .L1X    SP,A0         ; A0 = SP  
    STW     .D2T1   A10,*+SP(_A10) ;
    
;------------------------------------------------------------;
;     Calculate a correct SP value for                       ;
;     the entry in the TCB.                                  ;
;     Put it in the TCB at the B15 position (that is the SP) ;
;------------------------------------------------------------;
||  addk     .s1    (_IRP),A0      ; A0 = SP + FRAME Correct SP for POP of TCB
    STW     .D2T1   A0,*+SP(_B15) ;  B15 = A0 = SP + FRAME
    
     
||  B       .S2     b3            ;  Branch to continue in the interrupt handler 
    STW     .D2T1   A11,*+SP(_A11) ; 
    STW     .D2T1   A12,*+SP(_A12)  ; 
    STW     .D2T1   A13,*+SP(_A13)  ; 
    STW     .D2T1   A14,*+SP(_A14)  ; 
    STW     .D2T1   A15,*+SP(_A15)  ; 

  


    
;-----------------------------------------------------------;
;     Operating System Interrupt handler Context task switch ;
;     Executing in always enabled interrupt mode
;-----------------------------------------------------------;

_OSIntCtxSw
;----------------------------------------;
;   Get the address of the Taskіs TCB    ;
;   of the task that is going to execute ;
;----------------------------------------;
;----------------------------;
;   Adjust the stack pointer for saving in the Current taskіs TCB;
;----------------------------;

    LDW     .D2T2   *+DP(_OSTCBCur),B0     ; B0 = OSTCBCur
||  mv       SP,B1                         ; b1 = SP
    nop     4
    b   _OSStartHighRdy
||  addk    .s2     (8),B1                ; save adjusted SP
    STW     .D2T2   B1,*B0                ;  OSTCBCur->OSTCBStkPtr = SP ;
    nop     4









;-----------------------------------------------------------;
;     Operating System Context task switch                  ;
; Executing in always enabled interrupt mode
;-----------------------------------------------------------;
	.align	32
_OSCtxSw_Fast

;
;  TAKES 40 cycles to execute
;  240 nano sec
;
    addk     .s2            (4 -_Framesize),SP   ; Push one Frame

    STW     .D2T2   B3,*+SP(_StartAddress)  ;  b3 start address
    STW     .D2T2   B3,*+SP(_B3)            ;  Save for cosmetical reasons


||  mvc      AMR,B0
    STW     .D2T2   B0,*+SP(_AMR)  ;  
||  mvc      CSR,B0
    STW     .D2T2   B0,*+SP(_CSR)  ;  
||  mvc      IER,B0
    STW     .D2T2   B0,*+SP(_IER)  ;  



    STW        A3,*+SP(_A3)  ; 
    LDW       *+DP(_OSTCBCur),B0     ; B0 = OSTCBCur

;-------------------------------------------;
;  Get the address of the TCB that is going ;
;  to execute                               ;
;-------------------------------------------;
    LDW     .D2T2   *+DP(_OSTCBHighRdy),B4 ; B4 = OSTCBHighRdy

    STW     .D2T1   A10,*+SP(_A10) ; 
    STW     .D2T1   A11,*+SP(_A11) ; 
    STW     .D2T1   A12,*+SP(_A12)  ; 
    STW     .D2T1   A13,*+SP(_A13)  ; 
    STW     .D2T1   A14,*+SP(_A14)  ; 
    STW     .D2T1   A15,*+SP(_A15)  ; 

;-------------------------------;
;     Set it as the current TCB ;
; OSTCBCur = OSTCBHighRdy
;-------------------------------;
    STW     .D2T2   B4,*+DP(_OSTCBCur)      

    STW             SP,*B0                  ; previous OSTCBCur->OSTCBStkPtr = SP ;

    STW     .D2T2   B10,*+SP(_B10)  ;  
||  MV              SP,B10                   ; B10 = SP  
    STW     .D2T2   B11,*+SP(_B11)  ;  
||  addk           (_IRP),B10       ;         Correct SP for POP of TCB
;--------------------------------------------------;
;   In the TCB is the taskіs stack pointer value   ;
;   Get it                                         ;
;--------------------------------------------------;
; SP = OSTCBHighRdy->OSTCBStkPtr    
    LDW     .D2T2   *B4,SP                 

    STW             B10,*+SP(_B15)  ;         Save correct SP
    STW     .D2T2   B12,*+SP(_B12)  ;  
    STW     .D2T2   B13,*+SP(_B13)  ;  
    STW     .D2T2   B14,*+SP(_B14)  ;  

; POP registers from new TCB 

    LDW     .D2T2   *+SP(_StartAddress),B3  ;  b3 start address

    LDW     .D2T1   *+SP(_A3),A3  ; 

    LDW     .D2T1   *+SP(_A10),A10 ; 
    LDW     .D2T1   *+SP(_A11),A11 ; 
    LDW     .D2T1   *+SP(_A12),A12  ; 
    LDW     .D2T1   *+SP(_A13),A13  ; 
    LDW     .D2T1   *+SP(_A14),A14  ; 
    LDW     .D2T1   *+SP(_A15),A15  ; 
    
    LDW     .D2T2   *+SP(_B10),B10  ;  
    LDW     .D2T2   *+SP(_B11),B11  ;  
    LDW     .D2T2   *+SP(_B12),B12  ;  
    LDW     .D2T2   *+SP(_B14),B14  ;  
    LDW     .D2T2   *+SP(_B13),B13  ;  
    LDW     .D2T2   *+SP(_B3),B3    ;  
    LDW     .D2T2   *+SP(_B15),B15  ;  
||  B       .S2     b3              ;  
	nop     5


	.align	32

;-----------------------------------------------------------;
;     Operating System Context task switch                  ;
; Executing in always enabled interrupt mode
;-----------------------------------------------------------;
; OSCtxSw a little bit slower then _OSCtxSw_Fast but secure
; As it saves al registers
; Maybe Takes 200 nanosec more
;
; takes 90  cycles             = 540 nanosec
; fast OSCtxSW takes 40 cycles = 240 nanosec
;
;  8000 swithes per second is thus 8000  * 300 nanosec
;  = 2.4 milli seconds lost per second


_OSCtxSw


    addk     .s2            (4 -_Framesize),SP   ; Push one Frame

    STW     .D2T2   B3,*+SP(_StartAddress)  ;  b3 start address
    
    STW     .D2T2   B0,*+SP(_B0)  ;  
||  mvc      AMR,B0
    STW     .D2T2   B0,*+SP(_AMR)  ;  
||  mvc      CSR,B0
    STW     .D2T2   B0,*+SP(_CSR)  ;  
||  mvc      IER,B0
    STW     .D2T2   B0,*+SP(_IER)  ;  


    
    STW     .D2T1   A0,*+SP(_A0)  ; 
||  MV      .L1X    SP,A0                   ; A0 = SP  
    STW     .D2T1   A1,*+SP(_A1)  ; 
||  addk     .s1    (_IRP),A0     ; Correct SP for POP of TCB
    STW     .D2T1   A0,*+SP(_B15) ;  Save correct SP


    STW     .D2T1   A2,*+SP(_A2)  ; 
    STW     .D2T1   A3,*+SP(_A3)  ; 
    STW     .D2T1   A4,*+SP(_A4)  ;    
    STW     .D2T1   A5,*+SP(_A5)  ;  
    STW     .D2T1   A6,*+SP(_A6)  ;  
    STW     .D2T1   A7,*+SP(_A7)  ;  
    STW     .D2T1   A8,*+SP(_A8)  ; 
    STW     .D2T1   A9,*+SP(_A9)  ; 
    STW     .D2T1   A10,*+SP(_A10) ; 
    STW     .D2T1   A11,*+SP(_A11) ; 
    STW     .D2T1   A12,*+SP(_A12)  ; 
    STW     .D2T1   A13,*+SP(_A13)  ; 
    STW     .D2T1   A14,*+SP(_A14)  ; 
    STW     .D2T1   A15,*+SP(_A15)  ; 

    LDW     .D2T2   *+DP(_OSTCBCur),B0     ; B0 = OSTCBCur
    STW     .D2T2   B1,*+SP(_B1)  ;  
    STW     .D2T2   B2,*+SP(_B2)  ;  
    STW     .D2T2   B3,*+SP(_B3)  ;  Save for cosmetical reasons
    STW     .D2T2   B4,*+SP(_B4)  ;  
    STW     .D2T2   B5,*+SP(_B5)  ;  
    STW     .D2T2   B6,*+SP(_B6)  ;  

;----------------------------------------------------------------;
;  Save the current task's stack pointer into the current task's ;
;  OS TCB                                                        ;
;----------------------------------------------------------------;
    STW             SP,*B0                  ; previous OSTCBCur->OSTCBStkPtr = SP ;
    STW     .D2T2   B7 ,*+SP(_B7 ) ;  
    STW     .D2T2   B8 ,*+SP(_B8 ) ;   
    STW     .D2T2   B9 ,*+SP(_B9 ) ;   
    STW     .D2T2   B10,*+SP(_B10)  ;  
    STW     .D2T2   B11,*+SP(_B11)  ;  
    STW     .D2T2   B12,*+SP(_B12)  ;  
    STW     .D2T2   B13,*+SP(_B13)  ;  
    STW     .D2T2   B14,*+SP(_B14)  ;  
;   STW     .D2T2   B15,*+SP(_B15)  ;  already saved by first statement

;Pop NEW current task's context



*****************************************************************
*                      START MULTITASKING
*                   void OSStartHighRdy(void)
*****************************************************************

*****************************************************************
*            PERFORM A CONTEXT SWITCH (From an ISR)
*                     void OSIntCtxSw(void)
* The current taskіs environment is already saved by c6201_save
* save the current stackpos in the TCB
*
*
*
*
*****************************************************************


_OSStartHighRdy


;---------------------------------------;
;  Call User definable OSTaskSwHook();  ;
;---------------------------------------;

           B       .S1     _OSTaskSwHook
           NOP             3
           MVKL    .S2     OSStartHighRdy_1,B3       
           MVKH    .S2     OSStartHighRdy_1,B3       

OSStartHighRdy_1

;-------------------------------------------;
;  Get the address of the TCB that is going ;
;  to execute                               ;
;-------------------------------------------;
    LDW     .D2T2   *+DP(_OSTCBHighRdy),B4  ; B4 = OSTCBHighRdy
    LDB     .D2T2   *+DP(_OSPrioHighRdy),B5 ; B5 = OSPrioHighRdy
    nop 3
;-------------------------------;
;     Set it as the current TCB ;
; OSTCBCur  = OSTCBHighRdy
; OSPrioCur = OSPrioHighRdy
;-------------------------------;
    STW     .D2T2   B4,*+DP(_OSTCBCur)      
    STB     .D2T2   B5,*+DP(_OSPrioCur)     
||  mvk      1,b1
    STB     .D2T2   B1,*+DP(_OSRunning)     
;--------------------------------------------------;
;   In the TCB is the taskіs stack pointer value   ;
;   Get it                                         ;
; Get the stack pointer of the task to resume
;--------------------------------------------------;
; StackPointer = OSTCBHighRdy->OSTCBStkPtr    
    LDW     .D2T2   *B4,SP                 
    nop    4
    
    


;Pop NEW current task's context
;-----------------------------------------------------------;
;     Interrupt handlers restore after interrupt 
;     before going back into the interrupted environment
;-----------------------------------------------------------;

_c6201_Restor
;--------------------------------------;
;    Restor executes in enabled mode ;
;--------------------------------------;
; ENABLED MODE 

    LDW     .D2T2   *+SP(_StartAddress),B3  ;  b3 start address
    LDW     .D2T2   *+SP(_AMR),B10          ;   B10 = AMR
    LDW     .D2T2   *+SP(_CSR),B9           ;   B9  = CSR
    LDW     .D2T2   *+SP(_IER),B8           ;   B8  = IER
    LDW     .D2T1   *+SP(_A0),A0  ; 
    
    LDW     .D2T1   *+SP(_A1),A1  ; 
    LDW     .D2T1   *+SP(_A2),A2  ; 
    LDW     .D2T1   *+SP(_A3),A3  ; 
    LDW     .D2T1   *+SP(_A4),A4  ;    
    LDW     .D2T1   *+SP(_A5),A5  ;  
    LDW     .D2T1   *+SP(_A6),A6  ;  

    LDW     .D2T1   *+SP(_A7),A7  ;  
    LDW     .D2T1   *+SP(_A8),A8  ; 
    LDW     .D2T1   *+SP(_A9),A9  ; 

    LDW     .D2T1   *+SP(_A10),A10 ; 
    LDW     .D2T1   *+SP(_A11),A11 ; 
    LDW     .D2T1   *+SP(_A12),A12  ; 
    LDW     .D2T1   *+SP(_A13),A13  ; 
    LDW     .D2T1   *+SP(_A14),A14  ; 
    LDW     .D2T1   *+SP(_A15),A15  ; 

    LDW     .D2T2   *+SP(_B0),B0  ;  
    LDW     .D2T2   *+SP(_B1),B1  ;  
    LDW     .D2T2   *+SP(_B2),B2  ;  
    LDW     .D2T2   *+SP(_B4),B4  ;  
    
    
    LDW     .D2T2   *+SP(_B5),B5  ;  
    LDW     .D2T2   *+SP(_B6),B6  ;  
    LDW     .D2T2   *+SP(_B7 ),B7  ;  
    
    LDW     .D2T2   *+SP(_B11),B11  ;  
    LDW     .D2T2   *+SP(_B12),B12  ;  
||  mvk     0,B13
    LDW     .D2T2   *+SP(_B14),B14  ;  
||  mvc      B13,IER
    
; DISABLED MODE NO INTERRUPTS ALLOWED     
; DISABLED MODE NO INTERRUPTS ALLOWED     

    LDW     .D2T2   *+SP(_B9 ),B9  ;   
||  or             1,b9,b9                   ; Set GIE 
    LDW     .D2T2   *+SP(_B10),B10  ;  
||  mvc      B9,CSR	     	                 ; set csr if IRP did not put a GIE  
    LDW     .D2T2   *+SP(_B3),B3  ;  
||  mvc              B10,AMR                   ; Restore AMR
    LDW     .D2T2   *+SP(_B13),B13 ;  
    LDW     .D2T2   *+SP(_B8 ),B8  ;   
    LDW     .D2T2   *+SP(_B15),B15  ;  
    B       .S2     b3             ;  
    mvc             B8,IER                    ; 
    nop      5




	.end





;*******************************************************************************
;*******************************************************************************
__stack .usect ".stack", 8, 8
        .text
        .global __STACK_SIZE
        .global ___cinit__
        .global $bss
        .global _main
;*******************************************************************************
;*******************************************************************************
        .global __auto_init
        CMPEQ    .L1    -1,A4,A1
  [ A1] B        .S1          0x1B1C8
        STW      .D2T1  A12,*SP--[0x4]
        STW      .D2T1  A11,*+SP[0x3]
        STW      .D2T1  A10,*+SP[0x2]
        || NOP
        || NOP
        || NOP
        MV       .S2    B3,B13
        || STW   .D2T2  B13,*+SP[0x1]
        || MV    .D1    A4,A11
        || [ A1]  MVK .S1  0xffffffff,A0
  [!A1] LDW      .D1T1  *A11++[0x1],A10
        || [ A1]  MVKH .S1 0xffff0000,A0
        MVK      .S1    0xfffffff8,A12
        NOP       3
        MV       .D1    A10,A1
  [!A1] B        .S1    0x1B1CC
  [!A1] MVK      .S1    0xffffffff,A0
  [!A1] MVKH     .S1    0xffff0000,A0
  [!A1] CMPEQ    .L1    -1,A0,A1
        NOP       2
        MVK      .S2    0xffffa4a0,B5
        MVKH     .S2    0x10000,B5
        B        .S2    B5
        LDW      .D1T1  *+A11[0x0],A4
        MVK      .S2    0xffffb198,B3
        MVKH     .S2    0x10000,B3
        ADD      .L2X   4,A11,B4
        MV       .D1    A10,A6
        ADD      .D1    A11,A10,A0
        ADD      .D1    A0,0x0B,A0
        AND      .S1    A12,A0,A11
        LDW      .D1T1  *A11++[0x1],A10
        NOP       4
        MV       .D1    A10,A1
 [ A1]  B        .S1    0x1B180
 [!A1]  MVK      .S1    0xffffffff,A0
 [ A1]  MVK      .S2    0xffffa4a0,B5
 [!A1]  MVKH     .S1    0xffff0000,A0
 [ A1]  MVKH     .S2    0x10000,B5
        NOP
        CMPEQ    .L1    -1,A0,A1
 [ A1]  B        .S1    0x1B204
 [!A1]  LDW      .D1T1  *+A0[0x0],A1
        NOP       4
 [!A1]  B        .S1    0x1B204
        MV       .D1    A0,A10
        NOP       4
        B        .S2X   A1
        MVK      .S2    0xffffb1f4,B3
        MVKH     .S2    0x10000,B30001B1F0 00004000            NOP           30001B1F4 00A83264            LDW.D1T1      *++A10[0x1],A10001B1F8 00006000            NOP           40001B1FC 80000090     [ A1]  B.S1          0x1B1E40001B200 00008000            NOP           50001B204 053C23E5            LDDW.D2T1     *+SP[0x1],A11:A100001B208 01B401A2  ||        MV.S2         B13,B30001B20C 000C0363            B.S2          B30001B210 06BC22E6  ||        LDW.D2T2      *+SP[0x1],B130001B214 063C92E4            LDW.D2T1      *++SP[0x4],A120001B218 00006000            NOP           40001B21C 00000000            
        NOP
;*******************************************************************************
; _c_int00
;*******************************************************************************
        .newblock
        .global _c_int00
_c_int00:
;-------------------------------------------------------------------------------
; SET UP THE STACK POINTER IN B15.
; THE STACK POINTER POINTS 1 WORD PAST THE TOP OF THE STACK, SO SUBTRACT
; 1 WORD FROM THE SIZE. ALSO THE SP MUST BE ALIGNED ON AN 8-BYTE BOUNDARY
;-------------------------------------------------------------------------------
        MVKL     .S2    __stack + __STACK_SIZE - 4, SP
        MVKH     .S2    __stack + __STACK_SIZE - 4, SP
        AND      .S2    ~7,SP,SP
;-------------------------------------------------------------------------------
; SET UP THE GLOBAL PAGE POINTER IN B14.
;-------------------------------------------------------------------------------
        MVKL     .S2    $bss,DP
        MVKH     .S2    $bss,DP
;-------------------------------------------------------------------------------
; SET UP FLOATING POINT REGISTERS FOR C70 ONLY
;-------------------------------------------------------------------------------
        .if .TMS320C6700 = 1
        ZERO     .D2    B4
        MVC      .S2    B4,FADCR
        MVC      .S2    B4,FMCR
        .endif
;-------------------------------------------------------------------------------
; CALL THE AUTOINITIALIZATION ROUTINE.
;-------------------------------------------------------------------------------
        B               __auto_init
        MVK      .S1    ___cinit__,A4
        MVK      .S2    rp0?,B3
        MVKH     .S1    ___cinit__,A4
        MVKH     .S2    rp0?,B3
        NOP
rp0?:
;-------------------------------------------------------------------------------
; CALL THE USER'S PROGRAM.
;-------------------------------------------------------------------------------
        B               _main
        MVK      .S2    rp1?,B3
        MVKH     .S2    rp1?,B3
        NOP       3
rp1?:
;-------------------------------------------------------------------------------
; IDLE
;-------------------------------------------------------------------------------
        B         __idle_task
        NOP       5


        .bss __int_vector_table, 4*16, 32
        .bss _schedtsk, 4, 32
        .bss _runtsk, 4, 32
;*******************************************************************************
        .text
_int_init:
        MVKL    .S1     __int_vector_table,A0
        MVKH    .S1     __int_vector_table,A0
        || ZERO    .D1     A3
        STW     .D1T1   A3,*A0
        STW     .D1T1   A3,*+A0(4)
        STW     .D1T1   A3,*+A0(8)
        STW     .D1T1   A3,*+A0(12)
        STW     .D1T1   A3,*+A0(16)
        STW     .D1T1   A3,*+A0(20)
        STW     .D1T1   A3,*+A0(24)
        STW     .D1T1   A3,*+A0(28)
        STW     .D1T1   A3,*+A0(32)
        STW     .D1T1   A3,*+A0(36)
        STW     .D1T1   A3,*+A0(40)
        STW     .D1T1   A3,*+A0(44)
        STW     .D1T1   A3,*+A0(48)
        STW     .D1T1   A3,*+A0(52)
        STW     .D1T1   A3,*+A0(56)
        STW     .D1T1   A3,*+A0(60)
        B       .S2     B3
        NOP             5
