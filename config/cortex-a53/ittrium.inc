/* TCB offsets for AArch64 LP64:
 * QUEUE tskque: 2 * 8 = 16
 * CTXB  tskctxb.sp @ 16, .pc @ 24
 */
	.set	TCB_sp, 16
	.set	TCB_pc, 24

	/* Context frame size: x0-x30 (31*8) + SPSR + ELR + pad = 272 */
	.set	CTX_FRAME_SIZE, 272
	.set	CTX_X30, 240
	.set	CTX_SPSR, 248
	.set	CTX_ELR, 256

	.set	IDLE_STACK_SIZE, 2048
