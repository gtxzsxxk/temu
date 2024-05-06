//
// Created by hanyuan on 2024/5/6.
//

#include "port/main_memory.h"

static uint8_t ram_ptr[RAM_SIZE];

inline uint8_t port_main_memory_read(uint32_t offset) {
    /* offset must be less than RAM_SIZE */
    return ram_ptr[offset];
}

inline void port_main_memory_write(uint32_t offset, uint8_t data) {
    ram_ptr[offset] = data;
}
