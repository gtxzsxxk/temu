//
// Created by hanyuan on 2024/2/10.
//
#include <stdint.h>
#include "zicsr.h"

uint32_t control_status_registers[CSR_SIZE];
uint8_t csr_privilege[CSR_SIZE];
uint8_t csr_index_remap[0x10] = {
        [0x1] = CSR_idx_sstatus,
        [0x3] = CSR_idx_mstatus,
        [0x5] = CSR_idx_scontext,
        [0xC] = CSR_idx_cycle,
        [0xF] = CSR_idx_mvendorid,
};
