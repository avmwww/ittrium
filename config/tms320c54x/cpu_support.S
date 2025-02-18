        .c_mode
        .align  1
        .include  "ittrium.h54"
        .mmregs

;*******************************************************************************
; Pointer to top of stack used in interrupts
        .bss __int_stack, INTERRUPT_STACK_LEN, 0, 1
__int_stack_top       .set __int_stack + INTERRUPT_STACK_LEN
;*******************************************************************************
;*******************************************************************************
        .text
        .global __dispatch
__dispatch:
        PSHM   ST0
        PSHM   ST1
        PSHM   AR0
        PSHM   AR1
        MVDM   *(_runtsk),   AR0
        MVDM   *(_schedtsk), AR1
        NOP
        CMPR   EQ, AR1
        BC     dispatch_exit, TC
        ; Save current task context
        PSHM   AR2
        PSHM   AR3
        PSHM   AR4
        PSHM   AR5
        PSHM   AR6
        PSHM   AR7
        PSHM   AL
        PSHM   AH
        PSHM   AG
        PSHM   BL
        PSHM   BH
        PSHM   BG
        PSHM   T
        PSHM   TRN
        PSHM   BK
        PSHM   BRC
        PSHM   RSA
        PSHM   REA
        ;PSHM   PMST
        ; Store Stack Pointer
        LDM    AR0, A
        BC     _dispatch_i, AEQ
        LDM    SP, A
        STL    A, *AR0(TCB_sp)
        ; Activate scheduled task
_dispatch_i:
        LDM  AR1, A
        MVDK   *(_schedtsk), *(_runtsk)
        BC     __int_idle, AEQ
        MVDK   *AR1(TCB_sp), SP
        ; Restore activated task context
_int_exit:
        ;POPM   PMST
        POPM   REA
        POPM   RSA
        POPM   BRC
        POPM   BK
        POPM   TRN
        POPM   T
        POPM   BG
        POPM   BH
        POPM   BL
        POPM   AG
        POPM   AH
        POPM   AL
        POPM   AR7
        POPM   AR6
        POPM   AR5
        POPM   AR4
        POPM   AR3
        POPM   AR2
dispatch_exit:
        POPM   AR1
        POPM   AR0
        POPM   ST1
        POPM   ST0
        RETE
;*******************************************************************************
; IDLE process
;*******************************************************************************
__int_idle:
        ; Stack pointer points to top of interrupt stack
        STM    #__int_stack_top, SP
        ; Emulate interrupt phase
        ST     #0x01, *(_int_nesting)
        ; Enable interrupts
        RSBX   INTM
idle_loop:
        IDLE   1
        ; Interrupt happens, to dispatch
        MVDM   *(_schedtsk),   AR1
        B      _dispatch_i
;*******************************************************************************
;*******************************************************************************
        .global __interrupt_handler
__interrupt_handler:
        PSHM   ST0
        PSHM   ST1
        PSHM   AR0
        PSHM   AR1
        PSHM   AR2
        PSHM   AR3
        PSHM   AR4
        PSHM   AR5
        PSHM   AR6
        PSHM   AR7
        PSHM   AL
        PSHM   AH
        PSHM   AG
        PSHM   BL
        PSHM   BH
        PSHM   BG
        PSHM   T
        PSHM   TRN
        PSHM   BK
        PSHM   BRC
        PSHM   RSA
        PSHM   REA
        ;PSHM   PMST
        ;
        LD     *(_int_nesting), A
        ADDM   #1, *(_int_nesting)
        BC     int_from_int, ANEQ
        ; Store Stack Pointer
        MVDM   *(_runtsk),   AR1
        LDM    SP, A
        STL    A, *AR1(TCB_sp)
        ; Stack pointer points to top of interrupt stack
        STM #__int_stack_top, SP
        ; Disable current interrupt
        PSHM   IMR
        MVDM   *(_int_vec_no), AR2
        STM    #IMR, AR3
        LD     *AR2(_int_mask_table), A
        AND    *AR3, A
        STL    A, *AR3
        LD     *AR2(_int_vector_table), A
        ; Enable interrupts
        RSBX   INTM
        ; Call interrupt handler
        CALA   A
        ; Disable interrupts
        SSBX   INTM
        ADDM   #-1, *(_int_nesting)
        ; Restore interrupt mask register
        POPM   IMR
        MVDM   *(_schedtsk),   AR1
        B _dispatch_i
;*******************************************************************************
; Interrupt happens from interrupt
;*******************************************************************************
int_from_int:
        ; Save stack pointer and align it
        LDM    SP, A
        ANDM   #0xFFFE, *(SP)
        NOP
        NOP
        PSHM   AL
        PSHM   IMR
        MVDM   *(_int_vec_no), AR2
        STM    #IMR, AR3
        LD     *AR2(_int_mask_table), A
        AND    *AR3, A
        STL    A, *AR3
        LD     *AR2(_int_vector_table), A
        ; Enable interrupts
        RSBX   INTM
        ; Call interrupt handler
        CALA   A
        ; Disable interrupts
        SSBX   INTM
        ADDM   #-1, *(_int_nesting)
        ; Restore interrupt mask register
        POPM   IMR
        ; Restore Stack pointer
        POPM   SP
        ; return to interrupt
        B _int_exit
;*******************************************************************************
; VECTOR TABLE
;*******************************************************************************
       .sect   "vectors"
       .align  0x80
       .global __vectors_table
__vectors_table:

RESET: B _c_int00
       NOP
       NOP

NMI:   RETE
       NOP
       NOP
       NOP

sint17: BD   __dispatch
        NOP
        NOP

sint18: BD   __interrupt_handler
        STM  #3, _int_vec_no

sint19: BD   __interrupt_handler
        STM  #4, _int_vec_no

sint20: BD   __interrupt_handler
        STM  #5, _int_vec_no

sint21: BD   __interrupt_handler
        STM  #6, _int_vec_no

sint22: BD   __interrupt_handler
        STM  #7, _int_vec_no

sint23: BD   __interrupt_handler
        STM  #8, _int_vec_no

sint24: BD   __interrupt_handler
        STM  #9, _int_vec_no

sint25: BD   __interrupt_handler
        STM  #10, _int_vec_no

sint26: BD   __interrupt_handler
        STM  #11, _int_vec_no

sint27: BD   __interrupt_handler
        STM  #12, _int_vec_no

sint28: BD   __interrupt_handler
        STM  #13, _int_vec_no

sint29: BD   __interrupt_handler
        STM  #14, _int_vec_no

sint30: BD   __interrupt_handler
        STM  #15, _int_vec_no

int0:  BD   __interrupt_handler
       STM  #16, _int_vec_no

int1:  BD   __interrupt_handler
       STM  #17, _int_vec_no

int2:  BD   __interrupt_handler
       STM  #18, _int_vec_no

tint0: BD  __interrupt_handler
       STM  #19, _int_vec_no

rint0: BD  __interrupt_handler
       STM  #20, _int_vec_no

xint0: BD  __interrupt_handler
       STM  #21, _int_vec_no

dma0:  BD  __interrupt_handler
       STM  #22, _int_vec_no

tint1: BD  __interrupt_handler
       STM  #23, _int_vec_no

int3:  BD  __interrupt_handler
       STM  #24, _int_vec_no

hpi:   BD  __interrupt_handler
       STM  #25, _int_vec_no

rint1: BD  __interrupt_handler
       STM  #26, _int_vec_no

xint1: BD  __interrupt_handler
       STM  #27, _int_vec_no

dma4:  BD  __interrupt_handler
       STM  #28, _int_vec_no

dma5:  BD  __interrupt_handler
       STM  #29, _int_vec_no

rsv0:  BD  __interrupt_handler
       STM  #30, _int_vec_no

rsv1:  BD  __interrupt_handler
       STM  #31, _int_vec_no
;*******************************************************************************
          
       .end
