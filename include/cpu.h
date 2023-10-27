//
// Created by hanyuan on 2023/10/25.
//

#ifndef TEMU_CPU_H
#define TEMU_CPU_H

#include "memory.h"
#include "elf_helper.h"
#include "register_file.h"

class cpu {
protected:
    memory RAM;
    memory ROM;
    register_file *registers;
    uint64_t entry_addr;
public:
    cpu(uint64_t ram_map_addr,
        uint64_t ram_size,
        uint64_t rom_map_addr,
        uint64_t rom_size)
            : RAM(ram_map_addr, ram_size), ROM(rom_map_addr, rom_size) {

    }

    virtual void boot() = 0;

    void decode();

    void load_elf(elf_helper &handler);

    uint64_t *memory_map(uint64_t *mem);
};


#endif //TEMU_CPU_H
