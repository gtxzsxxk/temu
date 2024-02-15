	.file	"test_5.c"
	.option nopic
	.attribute arch, "rv64i2p1_a2p1"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.globl	__mulsf3
	.globl	__extendsfdf2
	.globl	__floatsidf
	.globl	__divdf3
	.globl	__adddf3
	.globl	__fixdfsi
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-64
	sd	ra,56(sp)
	sd	s0,48(sp)
	sd	s1,40(sp)
	addi	s0,sp,64
	lui	a5,%hi(.LC0)
	lw	a5,%lo(.LC0)(a5)
	sw	a5,-36(s0)
	lui	a5,%hi(.LC1)
	lw	a5,%lo(.LC1)(a5)
	sw	a5,-40(s0)
	li	a5,114
	sw	a5,-44(s0)
	lui	a5,%hi(.LC2)
	ld	a5,%lo(.LC2)(a5)
	sd	a5,-56(s0)
	lw	a1,-40(s0)
	lw	a0,-36(s0)
	call	__mulsf3
	mv	a5,a0
	mv	a0,a5
	call	__extendsfdf2
	mv	s1,a0
	lw	a5,-44(s0)
	mv	a0,a5
	call	__floatsidf
	mv	a5,a0
	ld	a1,-56(s0)
	mv	a0,a5
	call	__divdf3
	mv	a5,a0
	mv	a1,a5
	mv	a0,s1
	call	__adddf3
	mv	a5,a0
	mv	a0,a5
	call	__fixdfsi
	mv	a5,a0
	sext.w	a5,a5
	mv	a0,a5
	ld	ra,56(sp)
	ld	s0,48(sp)
	ld	s1,40(sp)
	addi	sp,sp,64
	jr	ra
	.size	main, .-main
	.section	.rodata
	.align	2
.LC0:
	.word	1100795085
	.align	2
.LC1:
	.word	1090099610
	.align	3
.LC2:
	.word	1374389535
	.word	1074339512
	.ident	"GCC: (GNU) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
