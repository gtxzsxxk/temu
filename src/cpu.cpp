//
// Created by hanyuan on 2023/10/27.
//
#include "cpu.h"

void cpu::load_elf(elf_helper &handler) {
//    for()
}

uint64_t *cpu::memory_map(uint64_t *mem) {
    uint64_t *tmp;
    tmp = RAM.remap(mem);
    if (tmp != nullptr) {
        return tmp;
    }
    tmp = ROM.remap(mem);
    if (tmp != nullptr) {
        return tmp;
    }
    return nullptr;
}
