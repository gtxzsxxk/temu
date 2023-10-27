//
// Created by hanyuan on 2023/10/27.
//

#include "decode.h"

std::tuple<uint8_t, uint8_t, uint8_t,
        uint8_t, uint8_t, uint8_t> inst_decode_r(uint32_t inst) {
    uint8_t opcode = inst & 0x7f;
    uint8_t rd = (inst >> 7) & 0x1f;
    uint8_t funct3 = (inst >> 12) & 0x07;
    uint8_t rs1 = (inst >> 15) & 0x1f;
    uint8_t rs2 = (inst >> 20) & 0x1f;
    uint8_t funct7 = (inst >> 25) & 0x7f;
    return std::make_tuple(opcode, rd, funct3, rs1, rs2, funct7);
}

std::tuple<uint8_t, uint8_t, uint8_t,
        uint8_t, uint16_t> inst_decode_i(uint32_t inst) {
    uint8_t opcode = inst & 0x7f;
    uint8_t rd = (inst >> 7) & 0x1f;
    uint8_t funct3 = (inst >> 12) & 0x07;
    uint8_t rs1 = (inst >> 15) & 0x1f;
    uint16_t imm = (inst >> 20) & 0xfff;
    return std::make_tuple(opcode, rd, funct3, rs1, imm);
}

std::tuple<uint8_t, uint8_t, uint8_t,
        uint8_t, uint8_t, uint8_t> inst_decode_s(uint32_t inst) {
    uint8_t opcode = inst & 0x7f;
    uint8_t imm1 = (inst >> 7) & 0x1f;
    uint8_t funct3 = (inst >> 12) & 0x07;
    uint8_t rs1 = (inst >> 15) & 0x1f;
    uint8_t rs2 = (inst >> 20) & 0x1f;
    uint8_t imm2 = (inst >> 25) & 0x7f;
    return std::make_tuple(opcode, imm1, funct3, rs1, rs2, imm2);
}

std::tuple<uint8_t, uint8_t, uint8_t,
        uint8_t, uint8_t, uint8_t> inst_decode_b(uint32_t inst) {
    uint8_t opcode = inst & 0x7f;
    uint8_t imm1 = (inst >> 7) & 0x1f;
    uint8_t funct3 = (inst >> 12) & 0x07;
    uint8_t rs1 = (inst >> 15) & 0x1f;
    uint8_t rs2 = (inst >> 20) & 0x1f;
    uint8_t imm2 = (inst >> 25) & 0x7f;
    return std::make_tuple(opcode, imm1, funct3, rs1, rs2, imm2);
}

std::tuple<uint8_t, uint8_t, uint32_t> inst_decode_u(uint32_t inst) {
    uint8_t opcode = inst & 0x7f;
    uint8_t rd = (inst >> 7) & 0x1f;
    uint32_t imm = (inst >> 12) & 0xfffff;
    return std::make_tuple(opcode, rd, imm);
}

std::tuple<uint8_t, uint8_t, int32_t> inst_decode_j(uint32_t inst) {
    uint8_t opcode = inst & 0x7f;
    uint8_t rd = (inst >> 7) & 0x1f;
    uint32_t imm = (inst & 0xff000) + (((inst >> 20) & 0xfffffffe) << 11)
                   + (((inst >> 21) & 0x3ff) << 1);
    if (inst & 0x80000000) {
        imm += (1 << 20);
    }
    return std::make_tuple(opcode, rd, (int32_t) imm);
}

void inst_exec(uint32_t inst, register_file *registers, memory *RAM, uint64_t *program_counter) {
    uint8_t opcode = inst & 0x7f;
    switch (opcode) {
        case LUI: {
            auto res = inst_decode_u(inst);
            registers->write(std::get<1>(res), (std::get<2>(res) << 12) & 0xffffffff);
        }
            break;
        case AUIPC: {
            auto res = inst_decode_u(inst);
            registers->write(std::get<1>(res), (std::get<2>(res) << 12) & 0xffffffff + *program_counter);
        }
            break;
        case JAL: {
            auto res = inst_decode_j(inst);
            registers->write(std::get<1>(res), *program_counter + 4);
            *program_counter += std::get<2>(res);
        }
            break;
    }
}