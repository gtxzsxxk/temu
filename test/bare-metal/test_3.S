	.file	"test_3.c"
	.option nopic
	.attribute arch, "rv64i2p1_f2p2_d2p2_zicsr2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	addi	sp,sp,-224
	.cfi_def_cfa_offset 224
	sd	s0,216(sp)
	.cfi_offset 8, -8
	addi	s0,sp,224
	.cfi_def_cfa 8, 0
	li	a5,1
	sw	a5,-20(s0)
	li	a5,-1
	sw	a5,-24(s0)
	sw	zero,-28(s0)
	li	a5,5
	sw	a5,-40(s0)
	li	a5,2
	sw	a5,-44(s0)
	li	a5,1
	sw	a5,-136(s0)
	li	a5,2
	sw	a5,-132(s0)
	li	a5,3
	sw	a5,-128(s0)
	li	a5,5
	sw	a5,-124(s0)
	li	a5,4
	sw	a5,-120(s0)
	sw	zero,-32(s0)
	j	.L2
.L8:
	lw	a5,-32(s0)
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-120(a5)
	sw	a5,-48(s0)
	lw	a5,-20(s0)
	sw	a5,-36(s0)
	j	.L3
.L6:
	lw	a5,-24(s0)
	addiw	a5,a5,1
	sw	a5,-24(s0)
	lw	a5,-24(s0)
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-36(s0)
	sw	a4,-200(a5)
	lw	a5,-36(s0)
	addiw	a5,a5,1
	sw	a5,-20(s0)
	lw	a5,-28(s0)
	ori	a5,a5,1
	sw	a5,-28(s0)
	lw	a5,-28(s0)
	slliw	a5,a5,1
	sw	a5,-28(s0)
	lw	a5,-24(s0)
	mv	a4,a5
	lw	a5,-44(s0)
	sext.w	a4,a4
	sext.w	a5,a5
	blt	a4,a5,.L4
	li	a5,-1
	j	.L9
.L4:
	lw	a5,-36(s0)
	addiw	a5,a5,1
	sw	a5,-36(s0)
.L3:
	lw	a5,-36(s0)
	mv	a4,a5
	lw	a5,-48(s0)
	sext.w	a4,a4
	sext.w	a5,a5
	ble	a4,a5,.L6
	lw	a5,-24(s0)
	addiw	a4,a5,-1
	sw	a4,-24(s0)
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-200(a5)
	sw	a5,-52(s0)
	lw	a5,-52(s0)
	mv	a4,a5
	lw	a5,-48(s0)
	sext.w	a4,a4
	sext.w	a5,a5
	beq	a4,a5,.L7
	li	a5,-1
	j	.L9
.L7:
	lw	a5,-28(s0)
	slliw	a5,a5,1
	sw	a5,-28(s0)
	lw	a5,-32(s0)
	addiw	a5,a5,1
	sw	a5,-32(s0)
.L2:
	lw	a5,-32(s0)
	mv	a4,a5
	lw	a5,-40(s0)
	sext.w	a4,a4
	sext.w	a5,a5
	blt	a4,a5,.L8
	lw	a5,-28(s0)
.L9:
	mv	a0,a5
	ld	s0,216(sp)
	.cfi_restore 8
	.cfi_def_cfa 2, 224
	addi	sp,sp,224
	.cfi_def_cfa_offset 0
	jr	ra
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Buildroot 2023.08.2-dirty) 13.2.0"
	.section	.note.GNU-stack,"",@progbits
