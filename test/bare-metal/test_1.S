	.file	"test_1.c"
	.option pic
	.attribute arch, "rv64i2p1_f2p2_d2p2_zicsr2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.globl	b
	.data
	.align	2
	.type	b, @object
	.size	b, 4
b:
	.word	1
	.globl	__muldi3
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	addi	sp,sp,-32
	.cfi_def_cfa_offset 32
	sd	ra,24(sp)
	sd	s0,16(sp)
	.cfi_offset 1, -8
	.cfi_offset 8, -16
	addi	s0,sp,32
	.cfi_def_cfa 8, 0
	li	a5,3
	sw	a5,-20(s0)
	lla	a5,b
	lw	a5,0(a5)
	lw	a4,-20(s0)
	mv	a1,a4
	mv	a0,a5
	call	__muldi3@plt
	mv	a5,a0
	sext.w	a4,a5
	lla	a5,b
	lw	a5,0(a5)
	addw	a5,a4,a5
	sext.w	a5,a5
	mv	a0,a5
	ld	ra,24(sp)
	.cfi_restore 1
	ld	s0,16(sp)
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
