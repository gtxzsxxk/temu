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
        case RV32I_LUI:
            break;
    }
}

static uint32_t pow(uint32_t x, uint32_t y) {
    uint32_t ans = 1;
    for (uint32_t i = 0; i < y; i++) {
        ans *= x;
    }
    return ans;
}

static uint32_t extract(uint32_t inst, uint8_t start, uint8_t end) {
    return (inst >> start) & (pow(2, (end - start + 1)) - 1);
}

#define INST_EXT(end, begin)  extract(inst,begin,end)
#define INST_DEC(type, ...) decode_type_##type(inst, __VA_ARGS__)
#define DEC_FUNC(OPCODE) static void decode_func_##OPCODE(uint32_t inst)
#define SEXT(op, target_idx, source_idx) ((int32_t)(op << (target_idx - source_idx))) >> (target_idx - source_idx)

static void decode_type_r(uint32_t inst, uint8_t *rd, uint8_t *funct3, uint8_t *rs1, uint8_t *rs2, uint8_t *funct7) {
    *rd = INST_EXT(11, 7);
    *funct3 = INST_EXT(14, 12);
    *rs1 = INST_EXT(19, 15);
    *rs2 = INST_EXT(24, 20);
    *funct7 = INST_EXT(31, 25);
}

static void decode_type_i(uint32_t inst, uint8_t *rd, uint8_t *funct3, uint8_t *rs1, uint16_t *imm) {
    *rd = INST_EXT(11, 7);
    *funct3 = INST_EXT(14, 12);
    *rs1 = INST_EXT(19, 15);
    *imm = INST_EXT(31, 20);
}

static void decode_type_s(uint32_t inst, uint32_t *imm, uint8_t *funct3, uint8_t *rs1, uint8_t *rs2) {
    *imm = INST_EXT(11, 7) | (INST_EXT(31, 25) << 5);
    *funct3 = INST_EXT(14, 12);
    *rs1 = INST_EXT(19, 15);
    *rs2 = INST_EXT(24, 20);
}

static void decode_type_b(uint32_t inst, uint32_t *imm, uint8_t *funct3, uint8_t *rs1, uint8_t *rs2) {
    *imm = (INST_EXT(11, 8) << 1) |
           (INST_EXT(30, 25) << 5) |
           (((inst >> 7) & 0x01) << 11) |
           (((inst >> 31) & 0x01) << 12);
    *funct3 = INST_EXT(14, 12);
    *rs1 = INST_EXT(19, 15);
    *rs2 = INST_EXT(24, 20);
}

static void decode_type_u(uint32_t inst, uint32_t *imm, uint8_t *rd) {
    *imm = (INST_EXT(31, 12) << 12);
    *rd = INST_EXT(11, 7);
}

static void decode_type_j(uint32_t inst, uint32_t *imm, uint8_t *rd) {
    *imm = (INST_EXT(19, 12) << 12) |
           (INST_EXT(30, 21) << 1) |
           (((inst >> 20) & 0x01) << 11) |
           (((inst >> 31) & 0x01) << 20);
    *rd = INST_EXT(11, 7);
}

DEC_FUNC(LUI) {
    uint32_t imm;
    uint8_t rd;
    INST_DEC(u, &imm, &rd);
    imm &= ~0x00000fff;
    registers[rd] = imm;
    program_counter += 4;
}

DEC_FUNC(AUIPC) {
    uint32_t imm;
    uint8_t rd;
    INST_DEC(u, &imm, &rd);
    imm &= ~0x00000fff;
    imm += program_counter;
    registers[rd] = imm;
    program_counter += 4;
}

DEC_FUNC(JAL) {
    uint32_t imm;
    uint8_t rd;
    INST_DEC(j, &imm, &rd);
    int32_t sext_imm = SEXT(imm, 31, 20);
    registers[rd] = program_counter + 4;
    program_counter += sext_imm;
}

DEC_FUNC(JALR) {
    uint16_t imm;
    uint8_t rs1;
    uint8_t funct3;
    uint8_t rd;
    INST_DEC(i, &rd, &funct3, &rs1, &imm);
    int32_t sext_offset = SEXT(imm, 31, 11);
    registers[rd] = program_counter + 4;
    program_counter = (registers[rs1] + sext_offset) & (~0x00000001);
}