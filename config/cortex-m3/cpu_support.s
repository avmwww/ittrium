    .include "ittrium.inc"
    .syntax unified
    .cpu cortex-m3
    .thumb

    .global runtsk
    .global schedtsk

    .global int_vector_table

    .global int_nesting
    .global _interrupt_handler

    .section .text
    .type SVC_Handler, %function
    .global SVC_Handler
    .thumb_func
    .type PendSV_Handler, %function
    .global PendSV_Handler
    .thumb_func
SVC_Handler:
PendSV_Handler:
    cpsid i
    ldr r2, =runtsk
    ldr r3, [r2]
    cbz r3, _dispatch
    tst lr, #4
    ite eq
    mrseq r0, msp
    mrsne r0, psp
    stmdb r0!, {r4-r11}
    mov r1, lr
    stmdb r0!, {r1}
    str r0, [r3, $TCB_sp]
.type _dispatch, %function
.thumb_func
_dispatch:
    ldr r3, =schedtsk
    ldr r1, [r3]
    str r1, [r2]
    cbz r1, idle_activate
restore_context:
    ldr r0, [r1, $TCB_sp]
    ldmia r0!, {r2}
    mov lr, r2
    ldmia r0!, {r4-r11}
    msr psp, r0
    mov r2, #0x02
    msr control, r2
    isb
    cpsie i
    bx lr

idle_activate:
    ldr  r0, =idle_stack
    adds r0, $IDLE_STACK_SIZE
    mov  r3, #0x01000000
    ldr  r2, =task_idle
    ldr  r1, =task_idle
    stmdb r0!, {r1-r3}
    mov  r5, #0x12
    mov  r4, #3
    mov  r3, #2
    mov  r2, #1
    mov  r1, #0
    stmdb r0!, {r1-r5}
    msr psp, r0
    cpsie i
    bx lr
    nop
task_idle:
    wfi
    b task_idle

    .type _interrupt_handler, %function
    .thumb_func
_interrupt_handler:
    cpsid i
    push {r4, lr}
    mov r4, r0
    ldr r1, =irq_count
    lsls r2, r4, #2
    ldr r3, [r1, r2]
    adds r3, #1
    str r3, [r1, r2]
    ldr r2, =int_nesting
    ldr r3, [r2]
    adds r3, #1
    str r3, [r2]
    ldr r1, =int_vector_table
    lsls r0, r4, #3
    adds r1, r0
    ldr r2, [r1]
    cbz r2, no_handler
    blx r2
no_handler:
    pop {r4, lr}
    ldr r2, =int_nesting
    ldr r3, [r2]
    subs r3, #1
    str r3, [r2]
    ldr r0, =schedtsk
    ldr r1, =runtsk
    ldr r0, [r0]
    ldr r1, [r1]
    cmp r0, r1
    beq int_ret
    ldr r0, =0xE000ED04
    ldr r1, =0x10000000
    str r1, [r0]
    dsb
    isb
int_ret:
    cpsie i
    bx lr
