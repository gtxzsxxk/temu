//
// Created by hanyuan on 2024/2/9.
//

#ifndef TEMU_DECODE_H
#define TEMU_DECODE_H
#include <stdint.h>

#define RV32I_LUI                               0x37
#define RV32I_AUIPC                             0x17
#define RV32I_JAL                               0x6f
#define RV32I_JALR                              0x67
#define RV32I_BRANCH                            0x63
#define RV32I_LOAD                              0x03
#define RV32I_STORE                             0x23
#define RV32I_ARITH_IMM                         0x13
#define RV32I_ARITH                             0x33
#define RV32I_ZIFENCEI_FENCE                    0x0f
#define RV32I_ZICSR_ECALL_EBREAK                0x73
#define RV32I_ATOMIC                            0x2f

#define INST_EXT(end, begin)  extract(inst,begin,end)
#define INST_DEC(type, ...) decode_type_##type(inst, __VA_ARGS__)
#define DEC_FUNC(OPCODE) static inline void decode_func_##OPCODE(uint32_t inst)
#define DECODE(OPCODE) case RV32I_##OPCODE: decode_func_##OPCODE(inst); break;
#define SEXT(op, target_idx, source_idx) (((int32_t)(op << (target_idx - source_idx))) >> (target_idx - source_idx))

void decode(uint32_t inst);

#endif //TEMU_DECODE_H
