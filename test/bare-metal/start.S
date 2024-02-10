    .global _start
_start:
    la gp, __global_pointer
    la sp, _estack

    la a0, _sdata
    la a1, _edata
    la t0, _sidata
    beq a0, a1, _bss
_copy_data:
    lw t1,0(t0)
    sw t1,0(a0)
    addi t0, t0, 4
    addi a0, a0, 4
    bne a0, a1, _copy_data

_bss:
    la a0, __bss_start__
    la a1, __bss_end__
    la t0, _sibss
    beq a0, a1, _entry
_copy_bss:
    lw t1,0(t0)
    lw t1,0(a0)
    addi t0, t0, 4
    addi a0, a0, 4
    bne a0, a1, _copy_bss

_entry:
    call main