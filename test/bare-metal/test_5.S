	.file	"test_5.c"
	.option nopic
	.attribute arch, "rv64i2p1_a2p1"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.globl	__floatsisf
	.globl	__mulsf3
	.globl	__fixsfsi
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-32
	sd	ra,24(sp)
	sd	s0,16(sp)
	addi	s0,sp,32
	lui	a5,%hi(.LC0)
	lw	a5,%lo(.LC0)(a5)
	sw	a5,-20(s0)
	li	a5,6
	sw	a5,-24(s0)
	lw	a5,-24(s0)
	mv	a0,a5
	call	__floatsisf
	mv	a5,a0
	lw	a1,-20(s0)
	mv	a0,a5
	call	__mulsf3
	mv	a5,a0
	mv	a0,a5
	call	__fixsfsi
	mv	a5,a0
	sext.w	a5,a5
	mv	a0,a5
	ld	ra,24(sp)
	ld	s0,16(sp)
	addi	sp,sp,32
	jr	ra
	.size	main, .-main
	.section	.rodata
	.align	2
.LC0:
	.word	1077936128
	.ident	"GCC: (GNU) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
