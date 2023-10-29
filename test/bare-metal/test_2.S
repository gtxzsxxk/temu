	.file	"test_2.c"
	.option pic
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
	addi	sp,sp,-32
	.cfi_def_cfa_offset 32
	sd	s0,24(sp)
	.cfi_offset 8, -8
	addi	s0,sp,32
	.cfi_def_cfa 8, 0
	li	a5,276
	sw	a5,-28(s0)
	li	a5,1300
	sw	a5,-24(s0)
	lw	a5,-28(s0)
	slliw	a5,a5,12
	sext.w	a5,a5
	lw	a4,-24(s0)
	or	a5,a5,a4
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	mv	a0,a5
	ld	s0,24(sp)
	.cfi_restore 8
	.cfi_def_cfa 2, 32
	addi	sp,sp,32
	.cfi_def_cfa_offset 0
	jr	ra
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Buildroot 2023.08.2-dirty) 13.2.0"
	.section	.note.GNU-stack,"",@progbits
