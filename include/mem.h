//
// Created by hanyuan on 2024/2/8.
//

#ifndef TEMU_MEM_H
#define TEMU_MEM_H
#include <stdint.h>
#include "parameters.h"

extern uint32_t program_counter;

uint8_t *mem_get_rom_ptr();
uint8_t mem_read_b(uint32_t addr);
uint16_t mem_read_h(uint32_t addr);
uint32_t mem_read_w(uint32_t addr);

uint32_t mem_register_read(uint8_t rd);
void mem_register_write(uint8_t rd, uint32_t value);

#endif //TEMU_MEM_H
