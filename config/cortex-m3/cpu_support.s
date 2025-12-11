        .include "ittrium.inc"
        .syntax unified
        .cpu cortex-m3
        .thumb

        .global _dispatch
        .global runtsk
        .global schedtsk

        .global int_vector_table
        .global interrupt_stack

        .global int_nesting
        .global interrupt_handler

        .section .text

interrupt_handler:
        // Disable interrupts
        cpsid i
        // Get vector callback
        ldr     r1, =int_vector_table
        ldr.w   r1, [r1, r0, lsl #3]
        // exit if callback not defined
        cbz     r1, int_exit
        // Switch to interrupt stack
        ldr  r0, =interrupt_stack
        adds r0, $INTERRUPT_STACK_SIZE
        // Increment interrupt nesting counter
        ldr     r3, =int_nesting
        ldr     r2, [r3, #0]
        adds    r2, #1
        str     r2, [r3, #0]
        push    {lr}
        // branch to handler
        blx     r1
        pop     {lr}
        // Decrement interrupt nesting counter
        ldr     r3, =int_nesting
        ldr     r2, [r3, #0]
        subs    r2, #1
        str     r2, [r3, #0]
        b       _dispatch
        nop

_dispatch:
        // Disable interrupts
        cpsid i
        //
        ldr   r0, =runtsk
        ldr   r1, [r0]
        cmp   r1, #0
        beq   skip_save_context
        // Save task context
        //mrs r0, psp
        mov r2, sp
        stmdb r2!, {r4-r11}
        str   r2, [r1, $TCB_sp]
skip_save_context:
        // move scheduled task to runnig state
        ldr   r2, =schedtsk
        ldr   r1, [r2]
        str   r1, [r0]
        cmp   r1, #0
        beq   task_idle_activate

restore_contex:
        // Task's SP
        ldr   r0, [r1, $TCB_sp]
int_nest_exit:
        ldmia r0!, {r4-r11}
        //msr psp, r0
        mov sp, r0
int_exit:
        cpsie i
        bx lr
        nop

        .global task_idle
task_idle_activate:
        // No active task
        // Make idle task context
        ldr  r0, =interrupt_stack
        adds r0, $INTERRUPT_STACK_SIZE
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
        mov sp, r0
        // Enable interrupts
        cpsie i
        // Branch to idle
        bx lr
        nop
task_idle:
        // wait for interrupt
        wfi
        b task_idle


