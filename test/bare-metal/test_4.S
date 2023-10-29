	.file	"test_4.c"
	.option nopic
	.attribute arch, "rv64i2p1_f2p2_d2p2_zicsr2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.globl	arr
	.bss
	.align	3
	.type	arr, @object
	.size	arr, 24
arr:
	.zero	24
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
	li	a5,999
	sw	a5,-20(s0)
	lui	a5,%hi(arr)
	addi	a5,a5,%lo(arr)
	li	a4,1
	sw	a4,0(a5)
	lui	a5,%hi(arr)
	addi	a5,a5,%lo(arr)
	li	a4,9
	sw	a4,4(a5)
	lui	a5,%hi(arr)
	addi	a5,a5,%lo(arr)
	li	a4,2
	sw	a4,8(a5)
	lui	a5,%hi(arr)
	addi	a5,a5,%lo(arr)
	li	a4,6
	sw	a4,12(a5)
	lui	a5,%hi(arr)
	addi	a5,a5,%lo(arr)
	li	a4,1
	sw	a4,16(a5)
	lui	a5,%hi(arr)
	addi	a5,a5,%lo(arr)
	li	a4,8
	sw	a4,20(a5)
	sw	zero,-24(s0)
	j	.L2
.L4:
	lui	a5,%hi(arr)
	addi	a4,a5,%lo(arr)
	lw	a5,-24(s0)
	slli	a5,a5,2
	add	a5,a4,a5
	lw	a4,0(a5)
	lw	a5,-20(s0)
	sext.w	a5,a5
	ble	a5,a4,.L3
	lui	a5,%hi(arr)
	addi	a4,a5,%lo(arr)
	lw	a5,-24(s0)
	slli	a5,a5,2
	add	a5,a4,a5
	lw	a5,0(a5)
	sw	a5,-20(s0)
.L3:
	lw	a5,-24(s0)
	addiw	a5,a5,1
	sw	a5,-24(s0)
.L2:
	lw	a5,-24(s0)
	sext.w	a4,a5
	li	a5,5
	ble	a4,a5,.L4
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
