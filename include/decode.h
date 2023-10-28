//
// Created by hanyuan on 2023/10/27.
//

#ifndef TEMU_DECODE_H
#define TEMU_DECODE_H

#include <tuple>
#include "cpu.h"

enum inst_opcode_rv32i {
    LUI = 0x37,
    AUIPC = 0x17,
    JAL = 0x6f,
    JALR = 0x67,
    BRANCH = 0x63,
    BRANCH_FUNCT_BEQ = 0,
    BRANCH_FUNCT_BNE = 1,
    BRANCH_FUNCT_BLT = 4,
    BRANCH_FUNCT_BGE = 5,
    BRANCH_FUNCT_BLTU = 6,
    BRANCH_FUNCT_BGEU = 7,
    L = 3,
    L_FUNCT_LB = 0,
    L_FUNCT_LH = 1,
    L_FUNCT_LW = 2,
    L_FUNCT_LBU = 4,
    L_FUNCT_LHU = 5,
    S = 0x23,
    S_FUNCT_SB = 0,
    S_FUNCT_SH = 1,
    S_FUNCT_SW = 2,
    ARITHMETIC_IMMEDIATE = 0x13,
    ARITH_FUNCT_ADDI = 0,
    ARITH_FUNCT_SLTI = 2,
    ARITH_FUNCT_SLTIU = 3,
    ARITH_FUNCT_XORI = 4,
    ARITH_FUNCT_ORI = 6,
    ARITH_FUNCT_ANDI = 7,
    ARITH_FUNCT_SLLI = 1,
    ARITH_FUNCT_SRLI = 5,
    ARITH_FUNCT_SRAI = 5,
    ARITH_FUNCT7_SLLI = 0,
    ARITH_FUNCT7_SRLI = 0,
    ARITH_FUNCT7_SRAI = 0x20,
    ARITHMETIC = 0x33,
    ARITH_FUNCT_ADD = 0,
    ARITH_FUNCT_SUB = 0,
    ARITH_FUNCT_SLL = 1,
    ARITH_FUNCT_SLT = 2,
    ARITH_FUNCT_SLTU = 3,
    ARITH_FUNCT_XOR = 4,
    ARITH_FUNCT_SRL = 5,
    ARITH_FUNCT_SRA = 5,
    ARITH_FUNCT_OR = 6,
    ARITH_FUNCT_AND = 7,
    ARITH_FUNCT7_ADD = 0,
    ARITH_FUNCT7_SUB = 0x20,
    ARITH_FUNCT7_SRL = 0,
    ARITH_FUNCT7_SRA = 0x20,
    FENCE_TSO_PAUSE = 0x0f,
    E_CALL_BREAK = 0x73
};

enum inst_opcode_rv64i {
    L_FUNCT_LWU = 6,
    L_FUNCt_LD = 3,
    S_FUNCT_SD = 3,
};

std::tuple<uint8_t, uint8_t, uint8_t,
        uint8_t, uint8_t, uint8_t> inst_decode_r(uint32_t inst);

std::tuple<uint8_t, uint8_t, uint8_t,
        uint8_t, int16_t> inst_decode_i(uint32_t inst);

std::tuple<uint8_t, uint8_t, uint8_t,
        uint8_t, int16_t> inst_decode_s(uint32_t inst);

std::tuple<uint8_t, uint8_t, uint8_t,
        uint8_t, int16_t> inst_decode_b(uint32_t inst);

std::tuple<uint8_t, uint8_t, uint32_t> inst_decode_u(uint32_t inst);

std::tuple<uint8_t, uint8_t, int32_t> inst_decode_j(uint32_t inst);

void inst_exec(uint32_t inst, cpu *machine);

#endif //TEMU_DECODE_H
