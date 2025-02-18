#include "ittrium.inc"
        NAME cpu_support

        EXTERN schedtsk, runtsk, interrupt_handler, int_nesting, _interrupt_vector
        PUBLIC _interrupt_handler

	; Load a stack-pointer with last even address of segment
LDSTACK	MACRO	segment,reg
	LDC	#sfe(segment), reg
	ENDM

	RSEG	ISTACK


        RSEG   CODE

_interrupt_handler:
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
        ; Restore task stack pointer
        LDC TCB_sp:8[A0], SP
int_nest_ret:
        ; Restore contex
        POPM  R0,R1,R2,R3,A0,A1,SB,FB
        REIT
;===============================================================================
_dispatch:
        ; Save current task context
        PUSHM  R0,R1,R2,R3,A0,A1,SB,FB
        MOV.W runtsk,   A1
        JZ skip_save_context
        STC SP, TCB_sp:8[A1]
skip_save_context:
        MOV.W schedtsk, A0
        ; Move scheduled task to runnig state
        MOV.W A0, runtsk
        JNZ restore_contex
;===============================================================================
; IDLE task
;===============================================================================
_int_idle:
        ; Set up interrupt stack
        LDSTACK ISTACK, ISP
        ; Select interrupt stack
        FCLR U
        ; Set interrupt priority level to 0
        LDIPL  #0
        NOP
        NOP
        NOP
        ; Allow interrupts
        FSET I
_int_idle_loop:
        ; Wait for interrupt
        ;WAIT
        JMP.B _int_idle_loop
;===============================================================================
_interrupt_handler_1:
        MOV.W #1, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_2:
        MOV.W #2, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_3:
        MOV.W #3, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_4:
        MOV.W #4, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_5:
        MOV.W #5, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_6:
        MOV.W #6, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_7:
        MOV.W #7, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_8:
        MOV.W #8, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_9:
        MOV.W #9, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_10:
        MOV.W #10, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_11:
        MOV.W #11, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_12:
        MOV.W #12, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_13:
        MOV.W #13, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_14:
        MOV.W #14, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_15:
        MOV.W #15, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_16:
        MOV.W #16, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_17:
        MOV.W #17, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_18:
        MOV.W #18, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_19:
        MOV.W #19, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_20:
        MOV.W #20, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_21:
        MOV.W #21, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_22:
        MOV.W #22, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_23:
        MOV.W #23, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_24:
        MOV.W #24, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_25:
        MOV.W #25, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_26:
        MOV.W #26, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
; Timer B1
_interrupt_handler_27:
        REIT
;        MOV.W #27, _interrupt_vector
;        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_28:
        MOV.W #28, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_29:
        MOV.W #29, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_30:
        MOV.W #30, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_31:
        MOV.W #31, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_32:
        MOV.W #32, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_33:
        MOV.W #33, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_34:
        MOV.W #34, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_35:
        MOV.W #35, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_36:
        MOV.W #36, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_37:
        MOV.W #37, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_38:
        MOV.W #38, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_39:
        MOV.W #39, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_40:
        MOV.W #40, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_41:
        MOV.W #41, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_42:
        MOV.W #42, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_43:
        MOV.W #43, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_44:
        MOV.W #44, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_45:
        MOV.W #45, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_46:
        MOV.W #46, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_47:
        MOV.W #47, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_48:
        MOV.W #48, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_49:
        MOV.W #49, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_50:
        MOV.W #50, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_51:
        MOV.W #51, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_52:
        MOV.W #52, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_53:
        MOV.W #53, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_54:
        MOV.W #54, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_55:
        MOV.W #55, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_56:
        MOV.W #56, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_57:
        MOV.W #57, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_58:
        MOV.W #58, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_59:
        MOV.W #59, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_60:
        MOV.W #60, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_61:
        MOV.W #61, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_62:
        MOV.W #62, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
_interrupt_handler_63:
        MOV.W #63, _interrupt_vector
        JMP.W _interrupt_handler
;===============================================================================
        COMMON      INTVEC:ROOT

        DC24 _dispatch
        DS8  1
        DC24 _interrupt_handler_1
        DS8  1
        DC24 _interrupt_handler_2
        DS8  1
        DC24 _interrupt_handler_3
        DS8  1
        DC24 _interrupt_handler_4
        DS8  1
        DC24 _interrupt_handler_5
        DS8  1
        DC24 _interrupt_handler_6
        DS8  1
        DC24 _interrupt_handler_7
        DS8  1
        DC24 _interrupt_handler_8
        DS8  1
        DC24 _interrupt_handler_9
        DS8  1
        DC24 _interrupt_handler_10
        DS8  1
        DC24 _interrupt_handler_11
        DS8  1
        DC24 _interrupt_handler_12
        DS8  1
        DC24 _interrupt_handler_13
        DS8  1
        DC24 _interrupt_handler_14
        DS8  1
        DC24 _interrupt_handler_15
        DS8  1
        DC24 _interrupt_handler_16
        DS8  1
        DC24 _interrupt_handler_17
        DS8  1
        DC24 _interrupt_handler_18
        DS8  1
        ;------------------------------------------------------------
        ; UART Interrupt Vectors setup for Renesas ROM-monitors.
        ;------------------------------------------------------------
        LWORD  0xFF900
;        DC24 _interrupt_handler_19
;        DS8  1
        LWORD  0xFF900
;        DC24 _interrupt_handler_20
;        DS8  1
        DC24 _interrupt_handler_21
        DS8  1
        DC24 _interrupt_handler_22
        DS8  1
        DC24 _interrupt_handler_23
        DS8  1
        DC24 _interrupt_handler_24
        DS8  1
        DC24 _interrupt_handler_25
        DS8  1
        DC24 _interrupt_handler_26
        DS8  1
        DC24 _interrupt_handler_27
        DS8  1
        DC24 _interrupt_handler_28
        DS8  1
        DC24 _interrupt_handler_29
        DS8  1
        DC24 _interrupt_handler_30
        DS8  1
        DC24 _interrupt_handler_31
        DS8  1
        DC24 _interrupt_handler_32
        DS8  1
        DC24 _interrupt_handler_33
        DS8  1
        DC24 _interrupt_handler_34
        DS8  1
        DC24 _interrupt_handler_35
        DS8  1
        DC24 _interrupt_handler_36
        DS8  1
        DC24 _interrupt_handler_37
        DS8  1
        DC24 _interrupt_handler_38
        DS8  1
        DC24 _interrupt_handler_39
        DS8  1
        DC24 _interrupt_handler_40
        DS8  1
        DC24 _interrupt_handler_41
        DS8  1
        DC24 _interrupt_handler_42
        DS8  1
        DC24 _interrupt_handler_43
        DS8  1
        DC24 _interrupt_handler_44
        DS8  1
        DC24 _interrupt_handler_45
        DS8  1
        DC24 _interrupt_handler_46
        DS8  1
        DC24 _interrupt_handler_47
        DS8  1
        DC24 _interrupt_handler_48
        DS8  1
        DC24 _interrupt_handler_49
        DS8  1
        DC24 _interrupt_handler_50
        DS8  1
        DC24 _interrupt_handler_51
        DS8  1
        DC24 _interrupt_handler_52
        DS8  1
        DC24 _interrupt_handler_53
        DS8  1
        DC24 _interrupt_handler_54
        DS8  1
        DC24 _interrupt_handler_55
        DS8  1
        DC24 _interrupt_handler_56
        DS8  1
        DC24 _interrupt_handler_57
        DS8  1
        DC24 _interrupt_handler_58
        DS8  1
        DC24 _interrupt_handler_59
        DS8  1
        DC24 _interrupt_handler_60
        DS8  1
        DC24 _interrupt_handler_61
        DS8  1
        DC24 _interrupt_handler_62
        DS8  1
        DC24 _interrupt_handler_63
        DS8  1

        END
