//
// Created by hanyuan on 2024/2/8.
//

#ifndef TEMU_MEM_H
#define TEMU_MEM_H
#include <stdint.h>
#include "parameters.h"

extern uint64_t registers[];
extern uint64_t program_counter;

uint8_t *mem_get_rom_ptr();
uint8_t mem_read_b(uint64_t addr);
uint16_t mem_read_h(uint64_t addr);
uint32_t mem_read_w(uint64_t addr);

#endif //TEMU_MEM_H
