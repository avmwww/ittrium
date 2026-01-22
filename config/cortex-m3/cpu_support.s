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
	.global _interrupt_handler

	.extern exit_task

	.section .text

// ============================================
// SVC Handler - вызывается через svc #0
// ============================================
	.type SVC_Handler, %function
	.global SVC_Handler
	.thumb_func
SVC_Handler:
    cpsid i
    // SVC автоматически сохранил: xPSR, PC, LR, R12, R3-R0 в стек PSP
    // Нам нужно сохранить R4-R11 и текущий PSP
    
    tst lr, #4           // Проверяем, какой стек использовался
    ite eq
    mrseq r0, msp        // Использовался MSP
    mrsne r0, psp        // Использовался PSP
    
    // Сохраняем R4-R11 в стек задачи
    stmdb r0!, {r4-r11}
    
    // Сохраняем LR (EXC_RETURN)
    mov r1, lr
    stmdb r0!, {r1}
    
    // Сохраняем SP в TCB текущей задачи
    ldr r2, =runtsk
    ldr r3, [r2]
    str r0, [r3, $TCB_sp]
    
    // Переходим к диспетчеру
    b _dispatch

// ============================================
// PendSV Handler - для отложенного переключения
// ============================================
.type PendSV_Handler, %function
.global PendSV_Handler
.thumb_func
PendSV_Handler:
    cpsid i
    // Сохраняем контекст текущей задачи
    tst lr, #4           // Проверяем, какой стек использовался
    ite eq
    mrseq r0, msp        // Использовался MSP
    mrsne r0, psp        // Использовался PSP

    // Сохраняем R4-R11 в стек задачи
    stmdb r0!, {r4-r11}

    // Сохраняем LR (EXC_RETURN)
    mov r1, lr
    stmdb r0!, {r1}
    
    // Сохраняем SP в TCB текущей задачи
    ldr r2, =runtsk
    ldr r3, [r2]
    str r0, [r3, $TCB_sp]
    
    b _dispatch

// ============================================
// Основной диспетчер
// ============================================
.type _dispatch, %function
.thumb_func
_dispatch:
    // Отключаем прерывания
    cpsid i
    
    // Получаем следующую задачу
    ldr r2, =schedtsk
    ldr r1, [r2]        // schedtsk -> r1
    ldr r0, =runtsk
    str r1, [r0]        // runtsk = schedtsk
    
    // Проверяем, есть ли задача для выполнения
    cbz r1, idle_task_activate
    
    // ============================================
    // Восстановление контекста новой задачи
    // ============================================
restore_context:
    // Загружаем SP новой задачи
    ldr r0, [r1, $TCB_sp]
    
    // Восстанавливаем LR (EXC_RETURN)
    ldmia r0!, {r2}     // LR -> r2
    mov lr, r2
    
    // Восстанавливаем R4-R11
    ldmia r0!, {r4-r11}
    
    // Восстанавливаем PSP или MSP
    tst lr, #4           // Проверяем, какой стек использовался
    ite eq
    msreq msp, r0        // Использовался MSP
    msrne psp, r0        // Использовался PSP
    
    // Настраиваем CONTROL для использования PSP
    // nPRIV (Bit 0):
    // Controls privilege level in Thread mode (0=privileged, 1=unprivileged).
    // SPSEL (Bit 1): Selects the stack pointer (0=MSP, 1=PSP).
    //mov r0, #0x03
    //msr control, r0
    isb
    
    // Включаем прерывания и возвращаемся
    cpsie i
    bx lr

// ============================================
// Активация idle задачи
// ============================================
idle_task_activate:
    // В этом коде idle задача уже должна быть создана
    // через make_task_context в C-части
    
    // Просто переходим в idle
    ldr r0, =runtsk
    ldr r1, [r0]
    cbz r1, no_task_panic
    
    // Idle задача есть, восстанавливаем её контекст
    b restore_context

no_task_panic:
    cpsie i
    // Нет ни одной задачи - паника
idle_loop:
    wfi
    b idle_loop

// ============================================
// Обработчик прерываний
// ============================================
	.type _interrupt_handler, %function
	.thumb_func
_interrupt_handler:
    cpsid i
    // r0 = номер вектора
    // Отключаем прерывания
    cpsid i

     // Сохраняем указатель стэка
    tst lr, #4           // Проверяем, какой стек использовался
    ite eq
    mrseq r2, msp        // Использовался MSP
    mrsne r2, psp        // Использовался PSP
  
    // Переключаемся на стек прерываний
    ldr r1, =interrupt_stack
    adds r1, $INTERRUPT_STACK_SIZE
    mov sp, r1

    push {r2}
    
    // Увеличиваем счётчик вложенности
    ldr r2, =int_nesting
    ldr r3, [r2]
    adds r3, #1
    str r3, [r2]
    
    // Сохраняем LR
    push {lr}
    
    // Ищем обработчик в таблице
    ldr r1, =int_vector_table
    lsls r0, r0, #3          // Каждая запись 8 байт
    adds r1, r0
    ldr r2, [r1]             // Функция обработчика
    cbz r2, no_handler
    
    // Вызываем обработчик
    blx r2

no_handler:
    // Восстанавливаем LR
    pop {lr}
    
    // Уменьшаем счётчик вложенности
    ldr r2, =int_nesting
    ldr r3, [r2]
    subs r3, #1
    str r3, [r2]
    
    // Проверяем, нужно ли переключение контекста
    ldr r0, =schedtsk
    ldr r1, =runtsk
    ldr r0, [r0]
    ldr r1, [r1]
    cmp r0, r1
    bne need_context_switch
int_ret:
    // Возвращаемся из прерывания
    pop {r2}
    // Восстанавливаем PSP или MSP
    tst lr, #4           // Проверяем, какой стек использовался
    ite eq
    msreq msp, r2        // Использовался MSP
    msrne psp, r2        // Использовался PSP
  
    cpsie i
    bx lr

need_context_switch:
    // Запрашиваем переключение через PendSV
    ldr r0, =0xE000ED04      // NVIC_INT_CTRL
    ldr r1, =0x10000000      // PendSV set
    str r1, [r0]
    dsb
    isb
    b int_ret
