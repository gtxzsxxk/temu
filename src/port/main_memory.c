//
// Created by hanyuan on 2024/5/6.
//

#include "port/main_memory.h"

#ifndef BARE_METAL_PLATFORM

static uint32_t ram_ptr[RAM_SIZE >> 2];
static uint8_t *ram_ptr_b = (uint8_t *) &ram_ptr;


inline uint32_t port_main_memory_read_w(uint32_t offset) {
    return ram_ptr[offset >> 2];
}

inline void port_main_memory_write_w(uint32_t offset, uint32_t data) {
    ram_ptr[offset >> 2] = data;
}

inline void port_main_memory_load_b(uint32_t offset, uint8_t data) {
    *(ram_ptr_b + offset) = data;
}

#else

#warning Define function port_main_memory_read_w outside of TEMU!
#warning Define function port_main_memory_write_w outside of TEMU!
#warning Define function port_main_memory_load_b outside of TEMU!

#endif
