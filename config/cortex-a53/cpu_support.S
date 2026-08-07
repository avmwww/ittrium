#include "ittrium.inc"

	.global	runtsk
	.global	schedtsk
	.global	int_nesting
	.global	idle_stack
	.global	dispatch
	.global	vectors_start
	.global	gic_acknowledge
	.global	gic_eoi
	.global	irq_handle

	.section	.text.vectors, "ax"
	.align	11
	.type	vectors_start, %function
vectors_start:
	/* Current EL with SP_ELx */
	.align	7
	b	sync_invalid		/* 0x000 Synchronous */
	.align	7
	b	irq_entry		/* 0x080 IRQ */
	.align	7
	b	fiq_invalid		/* 0x100 FIQ */
	.align	7
	b	serror_invalid		/* 0x180 SError */

	/* Lower EL using AArch64 */
	.align	7
	b	sync_invalid
	.align	7
	b	irq_entry
	.align	7
	b	fiq_invalid
	.align	7
	b	serror_invalid

	/* Lower EL using AArch32 */
	.align	7
	b	sync_invalid
	.align	7
	b	irq_entry
	.align	7
	b	fiq_invalid
	.align	7
	b	serror_invalid

	/* Current EL with SP_EL0 (unused) */
	.align	7
	b	sync_invalid
	.align	7
	b	irq_entry
	.align	7
	b	fiq_invalid
	.align	7
	b	serror_invalid

	.section	.text
	.align	2

/* --------------------------------------------------------------------
 * Save GPRs + SPSR/ELR onto current SP. Clobbers x0,x1 temporarily
 * after they are stacked; caller must have already reserved frame or
 * we allocate here.
 * Macro expects: SP already adjusted by CTX_FRAME_SIZE
 * -------------------------------------------------------------------- */
	.macro	save_gprs
	stp	x0,  x1,  [sp, #0]
	stp	x2,  x3,  [sp, #16]
	stp	x4,  x5,  [sp, #32]
	stp	x6,  x7,  [sp, #48]
	stp	x8,  x9,  [sp, #64]
	stp	x10, x11, [sp, #80]
	stp	x12, x13, [sp, #96]
	stp	x14, x15, [sp, #112]
	stp	x16, x17, [sp, #128]
	stp	x18, x19, [sp, #144]
	stp	x20, x21, [sp, #160]
	stp	x22, x23, [sp, #176]
	stp	x24, x25, [sp, #192]
	stp	x26, x27, [sp, #208]
	stp	x28, x29, [sp, #224]
	str	x30, [sp, #CTX_X30]
	.endm

	.macro	restore_gprs
	ldp	x0,  x1,  [sp, #0]
	ldp	x2,  x3,  [sp, #16]
	ldp	x4,  x5,  [sp, #32]
	ldp	x6,  x7,  [sp, #48]
	ldp	x8,  x9,  [sp, #64]
	ldp	x10, x11, [sp, #80]
	ldp	x12, x13, [sp, #96]
	ldp	x14, x15, [sp, #112]
	ldp	x16, x17, [sp, #128]
	ldp	x18, x19, [sp, #144]
	ldp	x20, x21, [sp, #160]
	ldp	x22, x23, [sp, #176]
	ldp	x24, x25, [sp, #192]
	ldp	x26, x27, [sp, #208]
	ldp	x28, x29, [sp, #224]
	ldr	x30, [sp, #CTX_X30]
	.endm

/* --------------------------------------------------------------------
 * Voluntary / forced dispatch from C
 * -------------------------------------------------------------------- */
	.type	dispatch, %function
dispatch:
	msr	daifset, #2

	sub	sp, sp, #CTX_FRAME_SIZE
	save_gprs

	/* SPSR = EL1h; ELR = return address (x30) */
	mov	x0, #0x5
	str	x0, [sp, #CTX_SPSR]
	str	x30, [sp, #CTX_ELR]

	/* Save SP into runtsk if present */
	adrp	x0, runtsk
	ldr	x1, [x0, :lo12:runtsk]
	cbz	x1, 1f
	mov	x2, sp
	str	x2, [x1, #TCB_sp]
1:
	b	restore_sched

/* Shared restore path: runtsk = schedtsk; restore or idle */
restore_sched:
	adrp	x0, schedtsk
	ldr	x1, [x0, :lo12:schedtsk]
	adrp	x0, runtsk
	str	x1, [x0, :lo12:runtsk]
	cbz	x1, idle_activate

	ldr	x0, [x1, #TCB_sp]
	mov	sp, x0

	ldr	x0, [sp, #CTX_SPSR]
	ldr	x1, [sp, #CTX_ELR]
	msr	spsr_el1, x0
	msr	elr_el1, x1
	restore_gprs
	add	sp, sp, #CTX_FRAME_SIZE
	eret

idle_activate:
	adrp	x0, idle_stack
	add	x0, x0, :lo12:idle_stack
	add	x0, x0, #IDLE_STACK_SIZE
	mov	sp, x0
	msr	daifclr, #2
idle_loop:
	wfi
	/* If a task became ready, switch to it */
	adrp	x0, schedtsk
	ldr	x1, [x0, :lo12:schedtsk]
	cbz	x1, idle_loop
	msr	daifset, #2
	b	restore_sched

/* --------------------------------------------------------------------
 * IRQ entry
 * -------------------------------------------------------------------- */
	.type	irq_entry, %function
irq_entry:
	sub	sp, sp, #CTX_FRAME_SIZE
	save_gprs
	mrs	x0, spsr_el1
	mrs	x1, elr_el1
	str	x0, [sp, #CTX_SPSR]
	str	x1, [sp, #CTX_ELR]

	/* Save task SP if runtsk set and not nested */
	adrp	x0, int_nesting
	ldr	w1, [x0, :lo12:int_nesting]
	cbnz	w1, 2f
	adrp	x0, runtsk
	ldr	x1, [x0, :lo12:runtsk]
	cbz	x1, 2f
	mov	x2, sp
	str	x2, [x1, #TCB_sp]
2:
	bl	gic_acknowledge
	/* x0 = IAR */
	mov	w19, w0			/* keep IAR in callee-saved */
	bl	irq_handle
	mov	w0, w19
	bl	gic_eoi

	/* Context switch if needed */
	adrp	x0, schedtsk
	ldr	x1, [x0, :lo12:schedtsk]
	adrp	x0, runtsk
	ldr	x2, [x0, :lo12:runtsk]
	cmp	x1, x2
	b.eq	irq_return
	/* Switch: SP already saved; just restore sched */
	b	restore_sched

irq_return:
	ldr	x0, [sp, #CTX_SPSR]
	ldr	x1, [sp, #CTX_ELR]
	msr	spsr_el1, x0
	msr	elr_el1, x1
	restore_gprs
	add	sp, sp, #CTX_FRAME_SIZE
	eret

sync_invalid:
	/* Print ESR/FAR via polled UART then halt */
	mrs	x0, esr_el1
	mrs	x1, far_el1
	mrs	x2, elr_el1
	bl	sync_abort_dump
1:	b	1b
fiq_invalid:
serror_invalid:
	b	.
