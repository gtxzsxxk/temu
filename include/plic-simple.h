//
// Created by hanyuan on 2024/3/12.
//

#ifndef TEMU_PLIC_SIMPLE_H
#define TEMU_PLIC_SIMPLE_H
#include <stdint.h>

#define PLIC_INT_LINE_UART      10

uint32_t plic_read_w(uint32_t offset);

void plic_write_w(uint32_t offset, uint32_t data);

void plic_throw_interrupt(uint32_t int_line);

void plic_clear_pending(uint32_t int_line);

#endif //TEMU_PLIC_SIMPLE_H
