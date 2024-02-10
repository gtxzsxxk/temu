//
// Created by hanyuan on 2024/2/8.
//
#include "mem.h"
#include "decode.h"

#define RISCV_ISA_TESTS

void machine_start(void) {
    program_counter = ROM_START_ADDR;
    for (;;) {
        uint32_t instruction = mem_read_w(program_counter);
        mem_debug_printreg(program_counter);
        decode(instruction);
#ifdef RISCV_ISA_TESTS
        if ((instruction & 0x7f) == RV32I_ZICSR_ECALL_EBREAK) {
            int a = 0;
        }
#endif
    }
}