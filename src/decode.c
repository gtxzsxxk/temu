//
// Created by hanyuan on 2024/2/8.
//
#include "decode.h"
#include "mem.h"
#include "zicsr.h"
#include "trap.h"

static uint32_t dec_pow(uint32_t x, uint32_t y) {
    uint32_t ans = 1;
    for (uint32_t i = 0; i < y; i++) {
        ans *= x;
    }
    return ans;
}

static uint32_t extract(uint32_t inst, uint8_t start, uint8_t end) {
    return (inst >> start) & (dec_pow(2, (end - start + 1)) - 1);
}

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
    mem_register_write(rd, imm);
    program_counter += 4;
}

DEC_FUNC(AUIPC) {
    uint32_t imm;
    uint8_t rd;
    INST_DEC(u, &imm, &rd);
    imm &= ~0x00000fff;
    imm += program_counter;
    mem_register_write(rd, imm);
    program_counter += 4;
}

DEC_FUNC(JAL) {
    uint32_t imm;
    uint8_t rd;
    INST_DEC(j, &imm, &rd);
    int32_t sext_imm = SEXT(imm, 31, 20);
    mem_register_write(rd, program_counter + 4);
    program_counter += sext_imm;
}

DEC_FUNC(JALR) {
    uint16_t imm;
    uint8_t rs1;
    uint8_t funct3;
    uint8_t rd;
    INST_DEC(i, &rd, &funct3, &rs1, &imm);
    int32_t sext_offset = SEXT(imm, 31, 11);
    /* potential bug: jalr x5, 0(x5) */
    uint32_t next_addr = program_counter + 4;
    program_counter = (mem_register_read(rs1) + sext_offset) & (~0x00000001);
    mem_register_write(rd, next_addr);
}

DEC_FUNC(BRANCH) {
    uint32_t imm;
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    INST_DEC(b, &imm, &funct3, &rs1, &rs2);
    int32_t sext_offset = SEXT(imm, 31, 12);
    switch (funct3) {
        case 0:
            /* BEQ */
            if (mem_register_read(rs1) == mem_register_read(rs2)) {
                program_counter += sext_offset;
            } else {
                program_counter += 4;
            }
            break;
        case 1:
            /* BNE */
            if (mem_register_read(rs1) != mem_register_read(rs2)) {
                program_counter += sext_offset;
            } else {
                program_counter += 4;
            }
            break;
        case 4:
            /* BLT */
            if (((int32_t) mem_register_read(rs1)) < ((int32_t) mem_register_read(rs2))) {
                program_counter += sext_offset;
            } else {
                program_counter += 4;
            }
            break;
        case 5:
            /* BGE */
            if (((int32_t) mem_register_read(rs1)) >= ((int32_t) mem_register_read(rs2))) {
                program_counter += sext_offset;
            } else {
                program_counter += 4;
            }
            break;
        case 6:
            /* BLTU */
            if ((mem_register_read(rs1)) < (mem_register_read(rs2))) {
                program_counter += sext_offset;
            } else {
                program_counter += 4;
            }
            break;
        case 7:
            /* BGEU */
            if ((mem_register_read(rs1)) >= (mem_register_read(rs2))) {
                program_counter += sext_offset;
            } else {
                program_counter += 4;
            }
            break;
        default:
            trap_throw_exception(EXCEPTION_ILLEGAL_INST);
            break;
    }
}

DEC_FUNC(LOAD) {
    uint8_t rd, funct3, rs1;
    uint16_t imm;
    INST_DEC(i, &rd, &funct3, &rs1, &imm);
    int32_t sext_offset = SEXT(imm, 31, 11);
    uint32_t target_addr = mem_register_read(rs1) + sext_offset;
    uint8_t access_error_intr = 0;
    if (funct3 == 0) {
        /* LB */
        uint8_t data = mem_read_b(target_addr, &access_error_intr);
        uint32_t sext_data = SEXT(data, 31, 7);
        mem_register_write(rd, sext_data);
    } else if (funct3 == 1) {
        /* LH */
        uint16_t data = mem_read_h(target_addr, &access_error_intr);
        uint32_t sext_data = SEXT(data, 31, 15);
        mem_register_write(rd, sext_data);
    } else if (funct3 == 2) {
        /* LW */
        uint32_t data = mem_read_w(target_addr, &access_error_intr);
        mem_register_write(rd, data);
    } else if (funct3 == 4) {
        /* LBU */
        uint8_t data = mem_read_b(target_addr, &access_error_intr);
        mem_register_write(rd, data);
    } else if (funct3 == 5) {
        /* LHU */
        uint16_t data = mem_read_h(target_addr, &access_error_intr);
        mem_register_write(rd, data);
    } else {
        trap_throw_exception(EXCEPTION_ILLEGAL_INST);
        program_counter -= 4;
    }

    if (access_error_intr) {
        trap_throw_exception(EXCEPTION_LOAD_ACCESS_FAULT);
    } else {
        program_counter += 4;
    }
}

DEC_FUNC(STORE) {
    uint32_t imm;
    uint8_t funct3, rs1, rs2;
    INST_DEC(s, &imm, &funct3, &rs1, &rs2);
    int32_t sext_offset = SEXT(imm, 31, 11);
    uint32_t target_addr = mem_register_read(rs1) + sext_offset;
    uint8_t access_error_intr = 0;
    if (funct3 == 0) {
        /* SB */
        mem_write_b(target_addr, mem_register_read(rs2) & 0xff, &access_error_intr);
    } else if (funct3 == 1) {
        /* SH */
        mem_write_h(target_addr, mem_register_read(rs2) & 0xffff, &access_error_intr);
    } else if (funct3 == 2) {
        /* SW */
        mem_write_w(target_addr, mem_register_read(rs2), &access_error_intr);
    } else {
        trap_throw_exception(EXCEPTION_ILLEGAL_INST);
        program_counter -= 4;
    }

    if (access_error_intr) {
        trap_throw_exception(EXCEPTION_STORE_ACCESS_FAULT);
    } else {
        program_counter += 4;
    }
}

DEC_FUNC(ARITH_IMM) {
    uint8_t rd, funct3, rs1;
    uint16_t imm;
    INST_DEC(i, &rd, &funct3, &rs1, &imm);
    int32_t sext_imm = SEXT(imm, 31, 11);
    if (funct3 == 0) {
        /* ADDI */
        mem_register_write(rd, mem_register_read(rs1) + sext_imm);
    } else if (funct3 == 2) {
        /* SLTI */
        mem_register_write(rd, (int32_t) mem_register_read(rs1) < sext_imm);
    } else if (funct3 == 3) {
        /* SLTIU */
        mem_register_write(rd, (uint32_t) mem_register_read(rs1) < (uint32_t) sext_imm);
    } else if (funct3 == 4) {
        /* XORI */
        mem_register_write(rd, (uint32_t) mem_register_read(rs1) ^ (uint32_t) sext_imm);
    } else if (funct3 == 6) {
        /* ORI */
        mem_register_write(rd, (uint32_t) mem_register_read(rs1) | (uint32_t) sext_imm);
    } else if (funct3 == 7) {
        /* ANDI */
        mem_register_write(rd, (uint32_t) mem_register_read(rs1) & (uint32_t) sext_imm);
    } else if (funct3 == 1) {
        /* SLLI */
        uint8_t shamt = imm & 0x1f;
        mem_register_write(rd, (uint32_t) mem_register_read(rs1) << shamt);
    } else if (funct3 == 5) {
        /* SRLI and SRAI */
        uint8_t shamt = imm & 0x1f;
        if (!(inst >> 30)) {
            /* SRLI */
            mem_register_write(rd, (uint32_t) mem_register_read(rs1) >> shamt);
        } else {
            /* SRAI */
            mem_register_write(rd, (int32_t) mem_register_read(rs1) >> shamt);
        }
    } else {
        trap_throw_exception(EXCEPTION_ILLEGAL_INST);
        program_counter -= 4;
    }

    program_counter += 4;
}

DEC_FUNC(ARITH) {
    uint8_t rd, funct3, rs1, rs2, funct7;
    INST_DEC(r, &rd, &funct3, &rs1, &rs2, &funct7);
    if (funct7 != 1) {
        if (funct3 == 0) {
            if (!(inst >> 30)) {
                /* ADD */
                mem_register_write(rd, mem_register_read(rs1) + mem_register_read(rs2));
            } else {
                /* SUB */
                mem_register_write(rd, mem_register_read(rs1) - mem_register_read(rs2));
            }
        } else if (funct3 == 1) {
            /* SLL */
            uint8_t shamt = mem_register_read(rs2) & 0x1f;
            mem_register_write(rd, mem_register_read(rs1) << shamt);
        } else if (funct3 == 2) {
            /* SLT */
            mem_register_write(rd, (int32_t) mem_register_read(rs1) < (int32_t) mem_register_read(rs2));
        } else if (funct3 == 3) {
            /* SLTU */
            mem_register_write(rd, mem_register_read(rs1) < mem_register_read(rs2));
        } else if (funct3 == 4) {
            /* XOR */
            mem_register_write(rd, mem_register_read(rs1) ^ mem_register_read(rs2));
        } else if (funct3 == 5) {
            /* SRL and SRA */
            uint8_t shamt = mem_register_read(rs2) & 0x1f;
            if (!(inst >> 30)) {
                /* SRL */
                mem_register_write(rd, (uint32_t) mem_register_read(rs1) >> shamt);
            } else {
                /* SRA */
                mem_register_write(rd, (int32_t) mem_register_read(rs1) >> shamt);
            }
        } else if (funct3 == 6) {
            /* OR */
            mem_register_write(rd, (uint32_t) mem_register_read(rs1) | (uint32_t) mem_register_read(rs2));
        } else if (funct3 == 7) {
            /* AND */
            mem_register_write(rd, (uint32_t) mem_register_read(rs1) & (uint32_t) mem_register_read(rs2));
        } else {
            trap_throw_exception(EXCEPTION_ILLEGAL_INST);
            program_counter -= 4;
        }
    } else {
        if (funct3 == 0) {
            /* MUL */
            mem_register_write(rd, mem_register_read(rs1) * mem_register_read(rs2));
        } else if (funct3 == 1) {
            /* MULH */
            int64_t res = (int64_t) ((int32_t) mem_register_read(rs1)) * (int64_t) ((int32_t) mem_register_read(rs2));
            mem_register_write(rd, ((uint64_t) res) >> 32);
        } else if (funct3 == 2) {
            /* MULHSU */
            int64_t res = (int64_t) ((int32_t) mem_register_read(rs1)) * ((uint32_t) mem_register_read(rs2));
            mem_register_write(rd, ((uint64_t) res) >> 32);
        } else if (funct3 == 3) {
            /* MULHU */
            uint64_t res = (uint64_t) mem_register_read(rs1) * (uint64_t) mem_register_read(rs2);
            mem_register_write(rd, res >> 32);
        } else if (funct3 == 4) {
            /* DIV */
            int32_t dividend = (int32_t) mem_register_read(rs1);
            int32_t divisor = (int32_t) mem_register_read(rs2);
            if (!divisor) {
                mem_register_write(rd, 0xffffffff);
            } else {
                if ((uint32_t) dividend == 0x80000000) {
                    mem_register_write(rd, dividend);
                } else {
                    int32_t res = dividend / divisor;
                    mem_register_write(rd, (uint32_t) res);
                }
            }
        } else if (funct3 == 5) {
            /* DIVU */
            uint32_t divisor = mem_register_read(rs2);
            if (!divisor) {
                mem_register_write(rd, 0xffffffff);
            } else {
                uint32_t res = mem_register_read(rs1) / divisor;
                mem_register_write(rd, (uint32_t) res);
            }
        } else if (funct3 == 6) {
            /* REM */
            int32_t dividend = (int32_t) mem_register_read(rs1);
            int32_t divisor = (int32_t) mem_register_read(rs2);
            if (!divisor) {
                mem_register_write(rd, dividend);
            } else {
                if ((uint32_t) dividend == 0x80000000) {
                    mem_register_write(rd, 0);
                } else {
                    int32_t res = dividend % divisor;
                    mem_register_write(rd, (uint32_t) res);
                }
            }
        } else if (funct3 == 7) {
            /* REMU */
            uint32_t dividend = mem_register_read(rs1);
            uint32_t divisor = mem_register_read(rs2);
            if (!divisor) {
                mem_register_write(rd, dividend);
            } else {
                uint32_t res = dividend % divisor;
                mem_register_write(rd, res);
            }
        }
    }

    program_counter += 4;
}

DEC_FUNC(ZICSR_ECALL_EBREAK) {
    uint8_t rd, funct3, rs1;
    uint16_t imm;
    INST_DEC(i, &rd, &funct3, &rs1, &imm);
    uint8_t illegal_inst_intr = 0;
    if (funct3 == 0x1) {
        /* CSRRW */
        csr_csrrw(rs1, rd, imm, &illegal_inst_intr);
    } else if (funct3 == 0x2) {
        /* CSRRS */
        csr_csrrs(rs1, rd, imm, &illegal_inst_intr);
    } else if (funct3 == 0x3) {
        /* CSRRC */
        csr_csrrc(rs1, rd, imm, &illegal_inst_intr);
    } else if (funct3 == 0x5) {
        /* CSRRWI */
        csr_csrrwi(rs1, rd, imm, &illegal_inst_intr);
    } else if (funct3 == 0x6) {
        /* CSRRSI */
        csr_csrrsi(rs1, rd, imm, &illegal_inst_intr);
    } else if (funct3 == 0x7) {
        /* CSRRCI */
        csr_csrrci(rs1, rd, imm, &illegal_inst_intr);
    } else if (!imm && !funct3 && !rd && !rs1) {
        /* ECALL */
        if (current_privilege == CSR_MASK_MACHINE) {
            trap_throw_exception(EXCEPTION_ECALL_FROM_M);
        } else if (current_privilege == CSR_MASK_SUPERVISOR) {
            trap_throw_exception(EXCEPTION_ECALL_FROM_S);
        } else if (current_privilege == CSR_MASK_USER) {
            trap_throw_exception(EXCEPTION_ECALL_FROM_U);
        } else {
            trap_throw_exception(EXCEPTION_ILLEGAL_INST);
        }
        program_counter -= 4;
    } else if (!funct3 && !rd && !rs1 && imm == 0x102) {
        /* SRET */
        trap_return_supervisor();
        /* avoid incorrect pc */
        program_counter -= 4;
    } else if (!funct3 && !rd && !rs1 && imm == 0x302) {
        /* MRET */
        trap_return_machine();
        /* avoid incorrect pc */
        program_counter -= 4;
    } else if (!funct3 && !rd && !rs1 && imm == 0x105) {
        /* TODO: WFI */
    }

    if (illegal_inst_intr) {
        trap_throw_exception(EXCEPTION_ILLEGAL_INST);
    } else {
        program_counter += 4;
    }
}

DEC_FUNC(ATOMIC) {
    uint8_t rd, funct3, rs1, rs2, funct7;
    INST_DEC(r, &rd, &funct3, &rs1, &rs2, &funct7);
    uint8_t typ = funct7 >> 2;
    uint8_t access_error_intr = 0;
    if (funct3 == 0x2) {
        uint32_t addr = mem_register_read(rs1);
        uint32_t value = mem_register_read(rs2);
        uint32_t t = mem_read_w(addr, &access_error_intr);
        if (access_error_intr) {
            trap_throw_exception(EXCEPTION_LOAD_ACCESS_FAULT);
            return;
        } else {
            mem_register_write(rd, t);
            uint32_t res = 0;
            if (typ == 0x01) {
                /* AMOSWAP.W */
                res = value;
            } else if (typ == 0x00) {
                /* AMOADD.W */
                res = t + value;
            } else if (typ == 0x04) {
                /* AMOXOR.W */
                res = t ^ value;
            } else if (typ == 0x0C) {
                /* AMOAND.W */
                res = t & value;
            } else if (typ == 0x08) {
                /* AMOOR.W */
                res = t | value;
            } else if (typ == 0x10) {
                /* AMOMIN.W */
                res = (int32_t) t < (int32_t) value ? t : value;
            } else if (typ == 0x14) {
                /* AMOMAX.W */
                res = (int32_t) t > (int32_t) value ? t : value;
            } else if (typ == 0x18) {
                /* AMOMINU.W */
                res = t < value ? t : value;
            } else if (typ == 0x1C) {
                /* AMOMAXU.W */
                res = t > value ? t : value;
            }
            mem_write_w(addr, res, &access_error_intr);
            if (access_error_intr) {
                trap_throw_exception(EXCEPTION_STORE_ACCESS_FAULT);
                return;
            }
        }
    } else {
        trap_throw_exception(EXCEPTION_ILLEGAL_INST);
        program_counter -= 4;
    }

    program_counter += 4;
}

DEC_FUNC(ZIFENCEI_FENCE) {
    program_counter += 4;
}

void decode(uint32_t inst) {
    uint8_t opcode = inst & 0x7f;
    switch (opcode) {
        DECODE(LUI)
        DECODE(AUIPC)
        DECODE(JAL)
        DECODE(JALR)
        DECODE(BRANCH)
        DECODE(LOAD)
        DECODE(STORE)
        DECODE(ARITH_IMM)
        DECODE(ARITH)
        DECODE(ZICSR_ECALL_EBREAK)
        DECODE(ATOMIC)
        DECODE(ZIFENCEI_FENCE)
        default:
            trap_throw_exception(EXCEPTION_ILLEGAL_INST);
            break;
    }
}