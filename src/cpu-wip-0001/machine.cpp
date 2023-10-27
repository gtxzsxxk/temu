//
// Created by hanyuan on 2023/10/25.
//

#include "machine.h"

void machine::boot() {
    registers->program_counter = entry_addr;
    while (1) {
        registers->program_counter += 4;
    }
}
