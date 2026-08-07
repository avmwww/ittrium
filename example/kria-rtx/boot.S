/*
 * Early boot: drop to EL1 if needed, no MMU/caches, jump to main.
 */
	.section	.text.boot, "ax"
	.global	_start
	.type	_start, %function
_start:
	/* Select SP_ELx */
	msr	spsel, #1

	mrs	x0, CurrentEL
	lsr	x0, x0, #2
	cmp	x0, #1
	b.eq	el1_entry
	cmp	x0, #2
	b.eq	drop_el2
	/* EL3 */
drop_el3:
	/* Lower levels are AArch64 */
	mrs	x0, scr_el3
	orr	x0, x0, #(1 << 10)	/* RW */
	orr	x0, x0, #(1 << 0)	/* NS */
	msr	scr_el3, x0
	msr	cptr_el3, xzr
	mov	x0, #0x5		/* EL1h */
	msr	spsr_el3, x0
	adr	x0, el1_entry
	msr	elr_el3, x0
	eret

drop_el2:
	mrs	x0, hcr_el2
	orr	x0, x0, #(1 << 31)	/* RW */
	msr	hcr_el2, x0
	mov	x0, #0x5
	msr	spsr_el2, x0
	adr	x0, el1_entry
	msr	elr_el2, x0
	eret

el1_entry:
	/* FP/ASIMD — gcc -Os may use Neon for copies */
	mov	x0, #(3 << 20)		/* CPACR_EL1.FPEN */
	msr	cpacr_el1, x0
	isb

	/* MMU/caches off */
	mrs	x0, sctlr_el1
	bic	x0, x0, #(1 << 0)	/* M */
	bic	x0, x0, #(1 << 2)	/* C */
	bic	x0, x0, #(1 << 12)	/* I */
	msr	sctlr_el1, x0
	isb

	/* Stack */
	adrp	x0, __stack_top
	add	x0, x0, :lo12:__stack_top
	mov	sp, x0

	/* Clear BSS */
	adrp	x1, __bss_start
	add	x1, x1, :lo12:__bss_start
	adrp	x2, __bss_end
	add	x2, x2, :lo12:__bss_end
1:
	cmp	x1, x2
	b.hs	2f
	str	xzr, [x1], #8
	b	1b
2:
	/* Mask IRQs until kernel enables timer */
	msr	daifset, #2

	mov	x0, #0
	mov	x1, #0
	bl	main
	b	.
