//
// Created by hanyuan on 2024/2/8.
//
#include "mem.h"

const uint8_t RV32I_LUI = 0x37;
const uint8_t RV32I_AUIPC = 0x17;
const uint8_t RV32I_JAL = 0x6f;
const uint8_t RV32I_JALR = 0x67;
const uint8_t RV32I_BRANCH = 0x63;
const uint8_t RV32I_LOAD = 0x03;
const uint8_t RV32I_STORE = 0x23;
const uint8_t RV32I_ARITH_IMM = 0x13;
const uint8_t RV32I_ARITH = 0x33;
const uint8_t RV32I_ZIFENCEI_FENCE = 0x0f;
const uint8_t RV32I_ZICSR_ECALL_EBREAK = 0x73;

const uint8_t RV32A = 0x2f;

/* 返回值是对pc指针的改变量 */
void decode(uint32_t inst) {
    uint8_t opcode = inst & 0x7f;
    switch (opcode) {
        
    }
}