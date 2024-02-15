//
// Created by hanyuan on 2024/2/10.
//
#include <stdint.h>
#include "zicsr.h"

uint32_t control_status_registers[CSR_SIZE];
uint8_t csr_privilege[CSR_SIZE];
uint8_t csr_index_remap[0xf] = {
        [0xC] = 0,
        []
};
