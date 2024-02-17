//
// Created by hanyuan on 2024/2/8.
//
#include <termios.h>
#include <unistd.h>
#include "machine.h"
#include "mem.h"
#include "decode.h"
#include "uart8250.h"
#include "trap.h"
#include "zicsr.h"

#define RISCV_DEBUG
#define RISCV_ISA_TESTS

static void set_terminal(void) {
    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
}

_Noreturn void machine_start(uint32_t start, int printreg) {
    uint8_t access_error_intr;
    program_counter = start;

    set_terminal();
    uart8250_init();

    for (;;) {
        access_error_intr = 0;
        uint32_t instruction = mem_read_w(program_counter, &access_error_intr);
        if (access_error_intr) {
            trap_throw_exception(EXCEPTION_INST_ACCESS_FAULT);
        } else {
#ifdef RISCV_DEBUG
            if (printreg) {
                mem_debug_printreg(program_counter);
            }
#ifdef RISCV_ISA_TESTS
            if (program_counter == 0x104) {
                int a = 0;
            }
            if (instruction == 0x00000073) {
                int a = 0;
            }
#endif
#endif

            decode(instruction);
        }
        uart8250_tick();
        zicnt_cycle_tick();

        if(zicnt_get_cycle() % SIM_YIELD_GAP == 0){
            zicnt_time_tick();
            usleep(SIM_YIELD_TIME);
        }
    }
}