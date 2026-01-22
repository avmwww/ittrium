    .include "ittrium.inc"
    .syntax unified
    .cpu cortex-m4
    .thumb

    .global runtsk
    .global schedtsk

    .global int_vector_table

    .global int_nesting
    .global _interrupt_handler

    .section .text
// ============================================
// SVC Handler - call from svc #0
// ============================================
    .type SVC_Handler, %function
    .global SVC_Handler
    .thumb_func
// ============================================
// PendSV Handler
// ============================================
    .type PendSV_Handler, %function
    .global PendSV_Handler
    .thumb_func
SVC_Handler:
PendSV_Handler:
    cpsid i
    // Test msp or psp
    tst lr, #4
    ite eq
    mrseq r0, msp
    mrsne r0, psp
    // Save R4-R11
    stmdb r0!, {r4-r11}
    // Store FPU state
    ldr r2, =runtsk
    ldr r3, [r2]
    ldr r1, [r3, $TCB_tskatr]
    tst r1, #TA_FPU
    beq store_cpu_regs
    // S16-S31
    vstmdb r0!, {s16-s31}
    // FPSCR
    vmrs r1, fpscr
    stmdb r0!, {r1}
store_cpu_regs:
    // Save LR (EXC_RETURN)
    mov r1, lr
    stmdb r0!, {r1}
    // Save SP of current task to TCB
    str r0, [r3, $TCB_sp]
    // clear bit FPCA
    mrs r1, control
    // Disable FPU
    bic r1, r1, #0x04
    msr control, r1
// ============================================
// Dispatch
// ============================================
.type _dispatch, %function
.thumb_func
_dispatch:
    ldr r3, =schedtsk
    ldr r1, [r3]        // schedtsk -> r1
    str r1, [r2]        // runtsk = schedtsk
    // check to scheduled task
    cbz r1, idle_activate
    // activate task, restore context
restore_context:
    ldr r0, [r1, $TCB_sp]
    // restore LR (EXC_RETURN)
    ldmia r0!, {r2}     // LR -> r2
    mov lr, r2
    // Restore FPU state, S16-S31, FPSCR
    ldr r2, [r1, $TCB_tskatr]
    tst r2, #TA_FPU
    beq restore_cpu_regs
    mov r2, #0x04
    msr control, r2
    // FPSCR
    ldmia r0!, {r2}
    vmsr fpscr, r2
    // S16-S31
    vldmia r0!, {s16-s31}
restore_cpu_regs:
    // restore R4-R11
    ldmia r0!, {r4-r11}
    // restore psp
    msr psp, r0
    // nPRIV (Bit 0):
    // Controls privilege level in Thread mode (0=privileged, 1=unprivileged).
    // SPSEL (Bit 1): Selects the stack pointer (0=MSP, 1=PSP).
    mrs r2, control
    orr r2, #0x02
    msr control, r2
    isb
    // enable interrupts and return
    cpsie i
    bx lr

// ============================================
// Activate idle
// ============================================
idle_activate:
    // No active task
    ldr  r0, =idle_stack
    adds r0, $IDLE_STACK_SIZE
    // xPSR, pc, lr
    mov  r3, #0x01000000
    ldr  r2, =task_idle
    ldr  r1, =task_idle
    stmdb r0!, {r1-r3}
    // r12, r3, r2, r1, r0
    mov  r5, #0x12
    mov  r4, #3
    mov  r3, #2
    mov  r2, #1
    mov  r1, #0
    stmdb r0!, {r1-r5}
    msr psp, r0
    // Enable interrupts
    cpsie i
    // Branch to idle
    bx lr
    nop
task_idle:
    // wait for interrupt
    wfi
    b task_idle

// ============================================
// interrupt handler
// ============================================
    .type _interrupt_handler, %function
    .thumb_func
_interrupt_handler:
    // r0 = vector number
    cpsid i
    // increment interrupts nesting counter
    ldr r2, =int_nesting
    ldr r3, [r2]
    adds r3, #1
    str r3, [r2]
    // saves lr
    push {lr}
    // find handler
    ldr r1, =int_vector_table
    lsls r0, r0, #3
    adds r1, r0
    ldr r2, [r1]
    cbz r2, no_handler
    // call handler
    blx r2
no_handler:
    // restore lr
    pop {lr}
    // decrement interrupts nesting counter
    ldr r2, =int_nesting
    ldr r3, [r2]
    subs r3, #1
    str r3, [r2]
    // check to context switch
    ldr r0, =schedtsk
    ldr r1, =runtsk
    ldr r0, [r0]
    ldr r1, [r1]
    cmp r0, r1
    beq int_ret
    // request by PendSV
    ldr r0, =0xE000ED04      // NVIC_INT_CTRL
    ldr r1, =0x10000000      // PendSV set
    str r1, [r0]
    dsb
    isb
int_ret:
    // return from interrupt
    cpsie i
    bx lr

