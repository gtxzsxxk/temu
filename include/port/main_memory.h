//
// Created by hanyuan on 2024/5/6.
//

#ifndef TEMU_MAIN_MEMORY_H
#define TEMU_MAIN_MEMORY_H

#include <stdint.h>
#include "parameters.h"

uint8_t port_main_memory_read(uint32_t offset);

void port_main_memory_write(uint32_t offset, uint8_t data);

#endif //TEMU_MAIN_MEMORY_H
