//
// Created by hanyuan on 2024/2/8.
//
#include "decode.h"
#include "mem.h"
#include "zicsr.h"
#include "trap.h"
#include "perf.h"

static uint32_t POWERS_OF_2[] = {
        1,
        2,
        4,
        8,
        16,
        32,
        64,
        128,
        256,
        512,
        1024,
        2048,
        4096,
        8192,
        16384,
        32768,
        65536,
        131072,
        262144,
        524288,
        1048576,
        2097152,
        4194304,
        8388608,
        16777216,
        33554432,
        67108864,
        134217728,
        268435456,
        536870912,
        1073741824,
        2147483648
};

static inline uint32_t extract(uint32_t inst, uint8_t start, uint8_t end) {
    return (inst >> start) & (POWERS_OF_2[(end - start + 1)] - 1);
}

static inline void decode_type_r(uint32_t inst, uint8_t *rd, uint8_t *funct3, uint8_t *rs1, uint8_t *rs2, uint8_t *funct7) {
    *rd = INST_EXT(11, 7);
    *funct3 = INST_EXT(14, 12);
    *rs1 = INST_EXT(19, 15);
    *rs2 = INST_EXT(24, 20);
    *funct7 = INST_EXT(31, 25);
}

static inline void decode_type_i(uint32_t inst, uint8_t *rd, uint8_t *funct3, uint8_t *rs1, uint16_t *imm) {
    *rd = INST_EXT(11, 7);
    *funct3 = INST_EXT(14, 12);
    *rs1 = INST_EXT(19, 15);
    *imm = INST_EXT(31, 20);
}

static inline void decode_type_s(uint32_t inst, uint32_t *imm, uint8_t *funct3, uint8_t *rs1, uint8_t *rs2) {
    *imm = INST_EXT(11, 7) | (INST_EXT(31, 25) << 5);
    *funct3 = INST_EXT(14, 12);
    *rs1 = INST_EXT(19, 15);
    *rs2 = INST_EXT(24, 20);
}

static inline void decode_type_b(uint32_t inst, uint32_t *imm, uint8_t *funct3, uint8_t *rs1, uint8_t *rs2) {
    *imm = (INST_EXT(11, 8) << 1) |
           (INST_EXT(30, 25) << 5) |
           (((inst >> 7) & 0x01) << 11) |
           (((inst >> 31) & 0x01) << 12);
    *funct3 = INST_EXT(14, 12);
    *rs1 = INST_EXT(19, 15);
    *rs2 = INST_EXT(24, 20);
}

static inline void decode_type_u(uint32_t inst, uint32_t *imm, uint8_t *rd) {
    *imm = (INST_EXT(31, 12) << 12);
    *rd = INST_EXT(11, 7);
}

static inline void decode_type_j(uint32_t inst, uint32_t *imm, uint8_t *rd) {
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
    PERF_MONITOR_CAN_MUTE("JAL", PERF_BATCH_10M);

    uint32_t imm;
    uint8_t rd;
    INST_DEC(j, &imm, &rd);
    int32_t sext_imm = SEXT(imm, 31, 20);
    mem_register_write(rd, program_counter + 4);
    program_counter += sext_imm;
}

DEC_FUNC(JALR) {
    PERF_MONITOR_CAN_MUTE("JALR", PERF_BATCH_10M);

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
    PERF_MONITOR_CAN_MUTE("BRANCH", PERF_BATCH_10M);

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
            trap_throw_exception(EXCEPTION_ILLEGAL_INST, inst);
            break;
    }
}

DEC_FUNC(LOAD) {
    PERF_MONITOR_CAN_MUTE("LOAD", PERF_BATCH_10M);

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
        if (!access_error_intr) {
            mem_register_write(rd, sext_data);
        }
    } else if (funct3 == 1) {
        /* LH */
        uint16_t data = mem_read_h(target_addr, &access_error_intr);
        uint32_t sext_data = SEXT(data, 31, 15);
        if (!access_error_intr) {
            mem_register_write(rd, sext_data);
        }
    } else if (funct3 == 2) {
        /* LW */
        uint32_t data = mem_read_w(target_addr, &access_error_intr);
        if (!access_error_intr) {
            mem_register_write(rd, data);
        }
    } else if (funct3 == 4) {
        /* LBU */
        uint8_t data = mem_read_b(target_addr, &access_error_intr);
        if (!access_error_intr) {
            mem_register_write(rd, data);
        }
    } else if (funct3 == 5) {
        /* LHU */
        uint16_t data = mem_read_h(target_addr, &access_error_intr);
        if (!access_error_intr) {
            mem_register_write(rd, data);
        }
    } else {
        trap_throw_exception(EXCEPTION_ILLEGAL_INST, inst);
        program_counter -= 4;
    }

    if (unlikely(access_error_intr)) {
        if (access_error_intr == 2) {
            trap_throw_exception(EXCEPTION_LOAD_PAGEFAULT, target_addr);
        } else if (access_error_intr == 3) {
            trap_throw_exception(EXCEPTION_LOAD_ADDR_MISALIGNED, target_addr);
        } else {
            trap_throw_exception(EXCEPTION_LOAD_ACCESS_FAULT, target_addr);
        }
    } else {
        program_counter += 4;
    }
}

DEC_FUNC(STORE) {
    PERF_MONITOR_CAN_MUTE("STORE", PERF_BATCH_10M);

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
        trap_throw_exception(EXCEPTION_ILLEGAL_INST, inst);
        program_counter -= 4;
    }

    if (unlikely(access_error_intr)) {
        if (access_error_intr == 2) {
            trap_throw_exception(EXCEPTION_STORE_PAGEFAULT, target_addr);
        } else if (access_error_intr == 3) {
            trap_throw_exception(EXCEPTION_STORE_ADDR_MISALIGNED, target_addr);
        } else {
            trap_throw_exception(EXCEPTION_STORE_ACCESS_FAULT, target_addr);
        }
    } else {
        program_counter += 4;
    }
}

DEC_FUNC(ARITH_IMM) {
    PERF_MONITOR_CAN_MUTE("ARITH for Immediate", PERF_BATCH_10M);

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
        trap_throw_exception(EXCEPTION_ILLEGAL_INST, inst);
        program_counter -= 4;
    }

    program_counter += 4;
}

DEC_FUNC(ARITH) {
    PERF_MONITOR_CAN_MUTE("ARITH", PERF_BATCH_10M);

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
            trap_throw_exception(EXCEPTION_ILLEGAL_INST, inst);
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
    PERF_MONITOR_CAN_MUTE("ECALL", PERF_BATCH_10M);

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
            trap_throw_exception(EXCEPTION_ECALL_FROM_M, 0);
        } else if (current_privilege == CSR_MASK_SUPERVISOR) {
            trap_throw_exception(EXCEPTION_ECALL_FROM_S, 0);
        } else if (current_privilege == CSR_MASK_USER) {
            trap_throw_exception(EXCEPTION_ECALL_FROM_U, 0);
        } else {
            trap_throw_exception(EXCEPTION_ILLEGAL_INST, inst);
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
        /* WFI */
        in_wfi = 1;
        wfi_next_pc = program_counter + 4;
        return;
    }

    if (unlikely(illegal_inst_intr)) {
        trap_throw_exception(EXCEPTION_ILLEGAL_INST, inst);
    } else {
        program_counter += 4;
    }
}

DEC_FUNC(ATOMIC) {
    /* TODO: implement the reservation set */
    PERF_MONITOR_CAN_MUTE("ATOMIC", PERF_BATCH_10M);

    static uint32_t reservation_set;
    static uint8_t reservation_valid = 0;
    uint8_t rd, funct3, rs1, rs2, funct7;
    INST_DEC(r, &rd, &funct3, &rs1, &rs2, &funct7);
    uint8_t typ = funct7 >> 2;
    uint8_t access_error_intr = 0;
    if (funct3 == 0x2) {
        uint32_t addr = mem_register_read(rs1);
        uint32_t value = mem_register_read(rs2);
        if (typ == 0x03) {
            /* SC.W */
            if (reservation_valid && reservation_set == addr) {
                reservation_valid = 0;
                mem_write_w(addr, value, &access_error_intr);
                if (access_error_intr) {
                    mem_register_write(rd, 1);
                    if (access_error_intr == 2) {
                        trap_throw_exception(EXCEPTION_STORE_PAGEFAULT, addr);
                    } else if (access_error_intr == 3) {
                        trap_throw_exception(EXCEPTION_STORE_ADDR_MISALIGNED, addr);
                    } else {
                        trap_throw_exception(EXCEPTION_STORE_ACCESS_FAULT, addr);
                    }
                    return;
                }
                mem_register_write(rd, 0);
            } else {
                mem_register_write(rd, 1);
                reservation_valid = 0;
            }
            program_counter += 4;
            return;
        }
        uint32_t t = mem_read_w(addr, &access_error_intr);
        if (access_error_intr) {
            trap_throw_exception(EXCEPTION_LOAD_ACCESS_FAULT, addr);
            return;
        } else {
            mem_register_write(rd, t);
            uint32_t res;
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
            } else if (typ == 0x02) {
                /* LR.W */
                reservation_valid = 1;
                reservation_set = addr;
                program_counter += 4;
                return;
            } else {
                trap_throw_exception(EXCEPTION_ILLEGAL_INST, inst);
                return;
            }
            mem_write_w(addr, res, &access_error_intr);
            if (access_error_intr) {
                trap_throw_exception(EXCEPTION_STORE_ACCESS_FAULT, addr);
                return;
            }
        }
    } else {
        trap_throw_exception(EXCEPTION_ILLEGAL_INST, inst);
        program_counter -= 4;
    }

    program_counter += 4;
}

DEC_FUNC(ZIFENCEI_FENCE) {
    program_counter += 4;
}

void decode(uint32_t inst) {
    PERF_MONITOR_CAN_MUTE("decode", PERF_BATCH_100M);

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
            trap_throw_exception(EXCEPTION_ILLEGAL_INST, inst);
            break;
    }
}
