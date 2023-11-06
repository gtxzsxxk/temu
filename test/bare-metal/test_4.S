	.file	"test_4.c"
	.option nopic
	.attribute arch, "rv64i2p1_a2p1"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.globl	__floatsidf
	.globl	__fixdfsi
	.globl	__muldi3
	.align	2
	.globl	DET
	.type	DET, @function
DET:
	addi	sp,sp,-48
	sd	ra,40(sp)
	sd	s0,32(sp)
	addi	s0,sp,48
	sd	a0,-40(s0)
	mv	a5,a1
	sw	a5,-44(s0)
	sw	zero,-24(s0)
	lw	a5,-44(s0)
	sext.w	a4,a5
	li	a5,1
	bne	a4,a5,.L2
	ld	a5,-40(s0)
	lw	a5,0(a5)
	j	.L3
.L2:
	lw	a5,-44(s0)
	sext.w	a4,a5
	li	a5,1
	ble	a4,a5,.L4
	sw	zero,-20(s0)
	j	.L5
.L6:
	lw	a4,-44(s0)
	lw	a5,-20(s0)
	mv	a2,a4
	mv	a1,a5
	ld	a0,-40(s0)
	call	Minor
	mv	a5,a0
	sw	a5,-28(s0)
	lw	a5,-20(s0)
	addiw	a5,a5,2
	sext.w	a5,a5
	mv	a0,a5
	call	__floatsidf
	mv	a4,a0
	lui	a5,%hi(.LC0)
	ld	a5,%lo(.LC0)(a5)
	mv	a1,a4
	mv	a0,a5
	call	pow
	mv	a5,a0
	mv	a0,a5
	call	__fixdfsi
	mv	a5,a0
	sext.w	a3,a5
	ld	a4,-40(s0)
	lw	a5,-20(s0)
	slli	a5,a5,2
	add	a5,a4,a5
	lw	a5,0(a5)
	mv	a1,a5
	mv	a0,a3
	call	__muldi3
	mv	a5,a0
	sext.w	a5,a5
	lw	a4,-28(s0)
	mv	a1,a4
	mv	a0,a5
	call	__muldi3
	mv	a5,a0
	sext.w	a5,a5
	lw	a4,-24(s0)
	addw	a5,a4,a5
	sw	a5,-24(s0)
	lw	a5,-20(s0)
	addiw	a5,a5,1
	sw	a5,-20(s0)
.L5:
	lw	a5,-20(s0)
	mv	a4,a5
	lw	a5,-44(s0)
	sext.w	a4,a4
	sext.w	a5,a5
	blt	a4,a5,.L6
.L4:
	lw	a5,-24(s0)
.L3:
	mv	a0,a5
	ld	ra,40(sp)
	ld	s0,32(sp)
	addi	sp,sp,48
	jr	ra
	.size	DET, .-DET
	.align	2
	.globl	Minor
	.type	Minor, @function
Minor:
	addi	sp,sp,-1648
	sd	ra,1640(sp)
	sd	s0,1632(sp)
	addi	s0,sp,1648
	sd	a0,-1640(s0)
	mv	a5,a1
	mv	a4,a2
	sw	a5,-1644(s0)
	mv	a5,a4
	sw	a5,-1648(s0)
	sw	zero,-20(s0)
	j	.L8
.L13:
	sw	zero,-24(s0)
	j	.L9
.L12:
	lw	a5,-24(s0)
	mv	a4,a5
	lw	a5,-1644(s0)
	sext.w	a4,a4
	sext.w	a5,a5
	bge	a4,a5,.L10
	lw	a5,-20(s0)
	addi	a4,a5,1
	mv	a5,a4
	slli	a5,a5,2
	add	a5,a5,a4
	slli	a5,a5,4
	mv	a4,a5
	ld	a5,-1640(s0)
	add	a4,a5,a4
	lw	a5,-24(s0)
	slli	a5,a5,2
	add	a5,a4,a5
	lw	a3,0(a5)
	lw	a2,-24(s0)
	lw	a4,-20(s0)
	mv	a5,a4
	slli	a5,a5,2
	add	a5,a5,a4
	slli	a5,a5,2
	add	a5,a5,a2
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	sw	a3,-1608(a5)
	j	.L11
.L10:
	lw	a5,-24(s0)
	mv	a4,a5
	lw	a5,-1644(s0)
	sext.w	a4,a4
	sext.w	a5,a5
	blt	a4,a5,.L11
	lw	a5,-20(s0)
	addi	a4,a5,1
	mv	a5,a4
	slli	a5,a5,2
	add	a5,a5,a4
	slli	a5,a5,4
	mv	a4,a5
	ld	a5,-1640(s0)
	add	a4,a5,a4
	lw	a5,-24(s0)
	addiw	a5,a5,1
	sext.w	a5,a5
	slli	a5,a5,2
	add	a5,a4,a5
	lw	a3,0(a5)
	lw	a2,-24(s0)
	lw	a4,-20(s0)
	mv	a5,a4
	slli	a5,a5,2
	add	a5,a5,a4
	slli	a5,a5,2
	add	a5,a5,a2
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	sw	a3,-1608(a5)
.L11:
	lw	a5,-24(s0)
	addiw	a5,a5,1
	sw	a5,-24(s0)
.L9:
	lw	a5,-1648(s0)
	addiw	a5,a5,-1
	sext.w	a4,a5
	lw	a5,-24(s0)
	sext.w	a5,a5
	blt	a5,a4,.L12
	lw	a5,-20(s0)
	addiw	a5,a5,1
	sw	a5,-20(s0)
.L8:
	lw	a5,-1648(s0)
	addiw	a5,a5,-1
	sext.w	a4,a5
	lw	a5,-20(s0)
	sext.w	a5,a5
	blt	a5,a4,.L13
	lw	a5,-1648(s0)
	addiw	a5,a5,-1
	sext.w	a4,a5
	addi	a5,s0,-1624
	mv	a1,a4
	mv	a0,a5
	call	DET
	mv	a5,a0
	mv	a0,a5
	ld	ra,1640(sp)
	ld	s0,1632(sp)
	addi	sp,sp,1648
	jr	ra
	.size	Minor, .-Minor
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-1616
	sd	ra,1608(sp)
	sd	s0,1600(sp)
	addi	s0,sp,1616
	addi	a5,s0,-1616
	li	a4,1600
	mv	a2,a4
	li	a1,0
	mv	a0,a5
	call	memset
	li	a5,1
	sw	a5,-1616(s0)
	li	a5,1
	sw	a5,-1612(s0)
	li	a5,4
	sw	a5,-1608(s0)
	li	a5,5
	sw	a5,-1536(s0)
	li	a5,1
	sw	a5,-1532(s0)
	li	a5,4
	sw	a5,-1528(s0)
	li	a5,9
	sw	a5,-1456(s0)
	li	a5,8
	sw	a5,-1452(s0)
	li	a5,5
	sw	a5,-1448(s0)
	addi	a5,s0,-1616
	li	a1,3
	mv	a0,a5
	call	DET
	mv	a5,a0
	mv	a0,a5
	ld	ra,1608(sp)
	ld	s0,1600(sp)
	addi	sp,sp,1616
	jr	ra
	.size	main, .-main
	.section	.rodata
	.align	3
.LC0:
	.word	0
	.word	-1074790400
	.ident	"GCC: (GNU) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
