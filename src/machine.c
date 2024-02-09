//
// Created by hanyuan on 2024/2/8.
//
#include "mem.h"
#include "decode.h"

void machine_start(void) {
    program_counter = ROM_START_ADDR;
    for(;;) {
        uint32_t instruction = mem_read_w(program_counter);
        decode(instruction);
        mem_debug_printreg();
    }
}