	.file	"test_4.c"
	.option nopic
	.attribute arch, "rv64i2p1_m2p0_a2p1"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.globl	data
	.section	.rodata
	.align	3
.LC0:
	.string	"Test of Qemu"
	.section	.sdata.data,"aw"
	.align	3
	.type	data, @object
	.size	data, 8
data:
	.dword	.LC0
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-16
	sd	ra,8(sp)
	sd	s0,0(sp)
	addi	s0,sp,16
	lui	a5,%hi(data)
	ld	a5,%lo(data)(a5)
	mv	a0,a5
	call	strlen
	mv	a5,a0
	sext.w	a5,a5
	mv	a0,a5
	ld	ra,8(sp)
	ld	s0,0(sp)
	addi	sp,sp,16
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
