INTERRUPT_STACK_LEN   SET 512
; TCB structure, offsets   queue(prev, next); 
TCB_sp  SET      8
TCB_pc  SET      16

TCB_task   set   16
TCB_stk    set   20
TCB_stksz  set   24
TCB_exinf  set   32

SYS32Mode     set   0x1F
CPSR_SVC      set   0x13
CPSR_IRQ_BIT  set   0x12
CPSR_FIQ_BIT  set   0x11


