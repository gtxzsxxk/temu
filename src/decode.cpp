//
// Created by hanyuan on 2023/10/27.
//

#include <iostream>
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
        uint8_t, int16_t> inst_decode_i(uint32_t inst) {
    uint8_t opcode = inst & 0x7f;
    uint8_t rd = (inst >> 7) & 0x1f;
    uint8_t funct3 = (inst >> 12) & 0x07;
    uint8_t rs1 = (inst >> 15) & 0x1f;
    uint16_t tmp = (inst >> 20) & 0xfff;
    auto imm = (int16_t) (tmp << 4);
    imm >>= 4;
    return std::make_tuple(opcode, rd, funct3, rs1, imm);
}

std::tuple<uint8_t, uint8_t, uint8_t,
        uint8_t, int16_t> inst_decode_s(uint32_t inst) {
    uint8_t opcode = inst & 0x7f;
    uint8_t funct3 = (inst >> 12) & 0x07;
    uint8_t rs1 = (inst >> 15) & 0x1f;
    uint8_t rs2 = (inst >> 20) & 0x1f;
    auto tmp = ((inst >> 7) & 0x1f) + ((inst >> 20) & 0xfe0);
    auto sext_offset = (int16_t) (tmp << 4);
    sext_offset >>= 4;
    return std::make_tuple(opcode, funct3, rs1, rs2, sext_offset);
}

std::tuple<uint8_t, uint8_t, uint8_t,
        uint8_t, int16_t> inst_decode_b(uint32_t inst) {
    uint8_t opcode = inst & 0x7f;
    uint8_t funct3 = (inst >> 12) & 0x07;
    uint8_t rs1 = (inst >> 15) & 0x1f;
    uint8_t rs2 = (inst >> 20) & 0x1f;
    uint16_t addr_tmp = ((inst >> 7) & 0x1e) + ((inst >> 20) & 0x7e0)
                        + ((inst << 4) & 0x800);
    if (inst & 0x80000000) {
        addr_tmp += (1 << 12);
    }
    auto addr = (int16_t) (addr_tmp << 3);
    addr >>= 3;

    return std::make_tuple(opcode, funct3, rs1, rs2, addr);
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
    uint32_t imm = (inst & 0xff000) + (((inst >> 20) & 0x01) << 11)
                   + (((inst >> 21) & 0x3ff) << 1);
    if (inst & 0x80000000) {
        imm += (1 << 20);
    }
    auto imm_sext = (int32_t) (imm << 11);
    imm_sext >>= 11;
    return std::make_tuple(opcode, rd, imm_sext);
}

int inst_exec(uint32_t inst, cpu *machine) {
    /* TODO: be compatible with different XLEN */
    uint8_t opcode = inst & 0x7f;
    register_file *registers = machine->get_registers();
    uint64_t *program_counter = &registers->program_counter;
    switch (opcode) {
        case LUI: {
            auto res = inst_decode_u(inst);
            registers->write(std::get<1>(res), (std::get<2>(res) << 12) & 0xffffffff);
        }
            break;
        case AUIPC: {
            auto res = inst_decode_u(inst);
            registers->write(std::get<1>(res),
                             ((((uint64_t) std::get<2>(res)) << 12) & 0xffffffff) + *program_counter);
        }
            break;
        case JAL: {
            auto res = inst_decode_j(inst);
            registers->write(std::get<1>(res), *program_counter + 4);
            *program_counter += std::get<2>(res);
        }
            break;
        case JALR: {
            /* std::make_tuple(opcode, rd, funct3, rs1, imm) */
            auto res = inst_decode_i(inst);
            uint64_t pc_next = registers->read(std::get<1>(res)) +
                               std::get<4>(res);
            pc_next &= 0xfffffffffffffffe;
            registers->write(std::get<1>(res), *program_counter + 4);
            *program_counter = pc_next;
        }
            break;
        case BRANCH: {
            /* return std::make_tuple(opcode, funct3, rs1, rs2, addr); */
            auto res = inst_decode_b(inst);
            auto funct3 = std::get<1>(res);
            switch (funct3) {
                case BRANCH_FUNCT_BEQ:
                    if (registers->read(std::get<2>(res)) ==
                        registers->read(std::get<3>(res))) {
                        *program_counter = std::get<4>(res);
                    }
                    break;
                case BRANCH_FUNCT_BNE:
                    if (registers->read(std::get<2>(res)) !=
                        registers->read(std::get<3>(res))) {
                        *program_counter = std::get<4>(res);
                    }
                    break;
                case BRANCH_FUNCT_BLT:
                    if (((int64_t) registers->read(std::get<2>(res))) <
                        ((int64_t) registers->read(std::get<3>(res)))) {
                        *program_counter = std::get<4>(res);
                    }
                    break;
                case BRANCH_FUNCT_BLTU:
                    if (registers->read(std::get<2>(res)) <
                        registers->read(std::get<3>(res))) {
                        *program_counter = std::get<4>(res);
                    }
                    break;
                case BRANCH_FUNCT_BGE:
                    if (((int64_t) registers->read(std::get<2>(res))) >=
                        ((int64_t) registers->read(std::get<3>(res)))) {
                        *program_counter = std::get<4>(res);
                    }
                    break;
                case BRANCH_FUNCT_BGEU:
                    if (registers->read(std::get<2>(res)) >=
                        registers->read(std::get<3>(res))) {
                        *program_counter = std::get<4>(res);
                    }
                    break;
                default:
                    std::cerr << "Unknown FUNCT3 when BRANCH: 0x" << std::hex << (int) funct3 << std::endl;
                    return -1;
            }
        }
            break;
        case L: {
            /* return std::make_tuple(opcode, rd, funct3, rs1, imm); */
            auto res = inst_decode_i(inst);
            auto funct3 = std::get<2>(res);
            uint64_t addr = registers->read(std::get<3>(res)) + std::get<4>(res);
            uint64_t *mem = machine->memory_map((uint64_t *) addr);
            uint64_t value = *mem;
            int64_t sext_tmp;
            switch (funct3) {
                case L_FUNCT_LB:
                    value &= 0xff;
                    sext_tmp = (int64_t) (value << 56);
                    sext_tmp >>= 56;
                    registers->write(std::get<1>(res), (uint64_t) sext_tmp);
                    break;
                case L_FUNCT_LBU:
                    value &= 0xff;
                    registers->write(std::get<1>(res), (uint64_t) value);
                    break;
                case L_FUNCT_LH:
                    value &= 0xffff;
                    sext_tmp = (int64_t) (value << 48);
                    sext_tmp >>= 48;
                    registers->write(std::get<1>(res), (uint64_t) sext_tmp);
                    break;
                case L_FUNCT_LHU:
                    value &= 0xffff;
                    registers->write(std::get<1>(res), (uint64_t) value);
                    break;
                case L_FUNCT_LW:
                    value &= 0xffffffff;
                    sext_tmp = (int64_t) (value << 32);
                    sext_tmp >>= 32;
                    registers->write(std::get<1>(res), (uint64_t) sext_tmp);
                    break;
                case L_FUNCT_LWU:
                    value &= 0xffffffff;
                    registers->write(std::get<1>(res), (uint64_t) value);
                    break;
                case L_FUNCt_LD:
                    registers->write(std::get<1>(res), (uint64_t) value);
                    break;
                default:
                    std::cerr << "Unknown FUNCT3 when LOAD: 0x" << std::hex << (int) funct3 << std::endl;
                    return -1;
            }
        }
            break;
        case S: {
            /* return std::make_tuple(opcode, funct3, rs1, rs2, sext_offset); */
            auto res = inst_decode_s(inst);
            auto funct3 = std::get<1>(res);
            uint64_t addr = registers->read(std::get<2>(res)) + std::get<4>(res);
            uint64_t *mem = machine->memory_map((uint64_t *) addr);
            switch (funct3) {
                case S_FUNCT_SB:
                    *mem &= 0xffffffffffffff00;
                    *mem |= (registers->read(std::get<3>(res)) & 0xff);
                    break;
                case S_FUNCT_SH:
                    *mem &= 0xffffffffffff0000;
                    *mem |= (registers->read(std::get<3>(res)) & 0xffff);
                    break;
                case S_FUNCT_SW:
                    *mem &= 0xffffffff00000000;
                    *mem |= (registers->read(std::get<3>(res)) & 0xffffffff);
                    break;
                case S_FUNCT_SD:
                    *mem = registers->read(std::get<3>(res));
                    break;
                default:
                    std::cerr << "Unknown store FUNCT: 0x" << std::hex << (int) funct3 << std::endl;
                    return -1;
            }
        }
            break;
        case ARITHMETIC_IMMEDIATE: {
            /* return std::make_tuple(opcode, rd, funct3, rs1, imm); */
            auto res = inst_decode_i(inst);
            uint8_t funct3 = std::get<2>(res);
            auto imm = std::get<4>(res);
            auto imm_64 = (int64_t) (imm << 48);
            imm_64 >>= 48;
            auto imm_u = (uint64_t) imm_64;
            uint8_t shamt = imm & 0x1f;
            switch (funct3) {
                case ARITH_FUNCT_ADDI:
                    registers->write(std::get<1>(res),
                                     registers->read(std::get<3>(res)) + imm);
                    break;
                case ARITH_FUNCT_SLTI:
                    if ((int64_t) registers->read(std::get<3>(res)) < imm) {
                        registers->write(std::get<1>(res), 1);
                    } else {
                        registers->write(std::get<1>(res), 0);
                    }
                    break;
                case ARITH_FUNCT_SLTIU:
                    if ((uint64_t) registers->read(std::get<3>(res)) < imm_u) {
                        registers->write(std::get<1>(res), 1);
                    } else {
                        registers->write(std::get<1>(res), 0);
                    }
                    break;
                case ARITH_FUNCT_XORI:
                    registers->write(std::get<1>(res),
                                     registers->read(std::get<3>(res)) ^ imm_64);
                    break;
                case ARITH_FUNCT_ORI:
                    registers->write(std::get<1>(res),
                                     registers->read(std::get<3>(res)) | imm_64);
                    break;
                case ARITH_FUNCT_ANDI:
                    registers->write(std::get<1>(res),
                                     registers->read(std::get<3>(res)) & imm_64);
                    break;
                case ARITH_FUNCT_SLLI:
                    registers->write(std::get<1>(res),
                                     ((uint64_t) registers->read(std::get<3>(res))) << shamt);
                    break;
                case ARITH_FUNCT_SRLI_SRAI:
                    if (inst & (1 << 30)) {
                        registers->write(std::get<1>(res),
                                         (uint64_t) (((int64_t) registers->read(std::get<3>(res))) >> shamt));
                    } else {
                        registers->write(std::get<1>(res),
                                         ((uint64_t) registers->read(std::get<3>(res))) >> shamt);
                    }
                    break;
                default:
                    std::cerr << "Unknown arith immediate funct: 0x" << std::hex << (int) funct3 << std::endl;
                    return -1;
            }
        }
            break;
        case ARITHMETIC: {
            /* return std::make_tuple(opcode, rd, funct3, rs1, rs2, funct7);*/
            auto res = inst_decode_r(inst);
            uint8_t funct3 = std::get<2>(res);
            uint8_t rd = std::get<1>(res);
            uint8_t rs1 = std::get<3>(res);
            uint8_t rs2 = std::get<4>(res);
            switch (funct3) {
                case ARITH_FUNCT_ADD_SUB:
                    if (inst & (1 << 30)) {
                        /* SUB */
                        registers->write(rd,
                                         registers->read(rs1) - registers->read(rs2));
                    } else {
                        /* ADD */
                        registers->write(rd,
                                         registers->read(rs1) + registers->read(rs2));
                    }
                    break;
                case ARITH_FUNCT_SLL:
                    registers->write(rd,
                                     registers->read(rs1)
                                             << (registers->read(rs2) & 0x1f));
                    break;
                case ARITH_FUNCT_SRL_SRA:
                    if (inst & (1 << 30)) {
                        registers->write(rd,
                                         (uint64_t) (((int64_t) registers->read(rs1))
                                                 >> (registers->read(rs2) & 0x1f)));
                    } else {
                        registers->write(rd,
                                         registers->read(rs1)
                                                 >> (registers->read(rs2) & 0x1f));
                    }
                    break;
                case ARITH_FUNCT_SLT:
                    if ((int64_t) registers->read(rs1) < (int64_t) registers->read(rs2)) {
                        registers->write(rd, 1);
                    } else {
                        registers->write(rd, 0);
                    }
                    break;
                case ARITH_FUNCT_SLTU:
                    if ((uint64_t) registers->read(rs1) < (uint64_t) registers->read(rs2)) {
                        registers->write(rd, 1);
                    } else {
                        registers->write(rd, 0);
                    }
                    break;
                case ARITH_FUNCT_XOR:
                    registers->write(rd,
                                     registers->read(rs1) ^ registers->read(rs2));
                    break;
                case ARITH_FUNCT_OR:
                    registers->write(rd,
                                     registers->read(rs1) | registers->read(rs2));
                    break;
                case ARITH_FUNCT_AND:
                    registers->write(rd,
                                     registers->read(rs1) & registers->read(rs2));
                    break;
                default:
                    std::cerr << "Unknown arith funct: 0x" << std::hex << (int) funct3 << std::endl;
                    return -1;
            }
        }
            break;
        case FENCE_TSO_PAUSE:
            /* Our I/O model are very simple. No need to implement this. */
            break;
        case SYSTEM:
            /* TODO: need to be done by the implementation of the interrupt */
            break;
        default:
            std::cerr << "Unknown OPCODE: 0x" << std::hex << (int) opcode << std::endl;
            return -1;
    }
    return 0;
}