//
// Created by hanyuan on 2023/10/25.
//

#include "machine.h"
#include "decode.h"

void machine::boot() {
    registers->program_counter = entry_addr;
    while (1) {
        uint32_t inst = (*memory_map((uint64_t *) registers->program_counter)) & 0xffffffff;
        inst_exec(inst, this);
        registers->program_counter += 4;
    }
}
