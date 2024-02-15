//
// Created by hanyuan on 2023/10/25.
//

#include <iostream>
#include "machine.h"
#include "decode.h"

void machine::boot() {
    registers->program_counter = entry_addr;
    while (1) {
        uint32_t inst = (*memory_map((uint64_t *) registers->program_counter)) & 0xffffffff;
        registers->debug_print();
        if (inst_exec(inst, this)) {
            break;
        }
        registers->program_counter += 4;
        if (registers->program_counter >= ROM.base_addr + text_size) {
            std::cerr << "Hard Fault: code end." << std::endl;
            break;
        }
        if (registers->program_counter < ROM.base_addr) {
            std::cerr << "Hard Fault: code cannot locate." << std::endl;
            break;
        }
    }
}
