	.cpu arm7tdmi
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.arm
	.syntax divided
	.file	"a.c"
	.section	.rodata
	.align	2
.LC0:
	.ascii	"hello world\012\000"
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	ldr	r3, .L3
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-8]
	ldr	r0, .L3+4
	mov	lr, pc
	bx	r3
	mov	r3, #0
	mov	r0, r3
	sub	sp, fp, #4
	@ sp needed
	ldmfd	sp!, {fp, lr}
	bx	lr
.L4:
	.align	2
.L3:
	.word	535822348
	.word	.LC0
	.size	_start, .-_start
	.ident	"GCC: (GNU) 5.2.0"
