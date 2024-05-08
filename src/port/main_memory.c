//
// Created by hanyuan on 2024/5/6.
//

#include "port/main_memory.h"

static uint32_t ram_ptr[RAM_SIZE >> 2];
static uint8_t *ram_ptr_b = (uint8_t *)&ram_ptr;

__attribute__((optimize("O0")));
static void mem_delay() {
    volatile uint32_t cnt = 1;
    while(cnt) cnt--;
}

inline uint32_t port_main_memory_read_w(uint32_t offset) {
    mem_delay();
    if(offset == 0x48CD8) {
        int a  = 0;
    }
    return ram_ptr[offset >> 2];
}

inline void port_main_memory_write_w(uint32_t offset, uint32_t data) {
    mem_delay();
    if(offset == 0x48CD8) {
        int a  = 0;
    }
    ram_ptr[offset >> 2] = data;
}

inline void port_main_memory_load_b(uint32_t offset, uint8_t data) {
    *(ram_ptr_b + offset) = data;
}
