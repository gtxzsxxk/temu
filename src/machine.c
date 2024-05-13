//
// Created by hanyuan on 2024/2/8.
//

#include "machine.h"
#include "parameters.h"
#include "mmu.h"
#include "decode.h"
#include "uart8250.h"
#include "trap.h"
#include "zicsr.h"
#include "port/console.h"
#include "perf.h"
#include "port/os_yield_cpu.h"

//#define RISCV_ISA_TESTS

static uint8_t access_error_intr;

static void machine_pre_boot(uint32_t start);

static void machine_tick(void);

static void machine_debug(uint32_t instruction, int printreg);

_Noreturn void machine_start(uint32_t start, int printreg) {
    uint32_t instruction;
    machine_pre_boot(start);

    for (;;) {
        PERF_MONITOR_CONTINUOUS(mainloop, PERF_BATCH_100M);

        access_error_intr = 0;
        instruction = mmu_read_inst(program_counter, &access_error_intr);
        if (unlikely(access_error_intr)) {
            if (access_error_intr == 2) {
                trap_throw_exception(EXCEPTION_INST_PAGEFAULT, program_counter);
            } else if (access_error_intr == 3) {
                trap_throw_exception(EXCEPTION_INST_ADDR_MISALIGNED, program_counter);
            } else {
                trap_throw_exception(EXCEPTION_INST_ACCESS_FAULT, program_counter);
            }
        } else {
            machine_debug(instruction, printreg);
            decode(instruction);
        }
        machine_tick();
    }
}

static void machine_pre_boot(uint32_t start) {
    program_counter = start;

    zicnt_init();

    port_os_console_init();
    port_os_yield_cpu_add_interrupt(zicnt_time_tick);
    port_os_yield_cpu_add_interrupt(uart8250_tick);
}

static void machine_tick(void) {
    zicnt_cycle_tick();

    if (zicnt_get_cycle() % ZICNT_TICK_INTERVAL == 0) {
        zicnt_time_tick();
    }

    if (zicnt_get_cycle() % (16) == 0) {
        uart8250_tick();
    }

    trap_take_interrupt();
}

static void machine_debug(uint32_t instruction, int printreg) {
#if TEMU_DEBUG_CODE
    if (printreg) {
        mmu_debug_printreg(program_counter);
    }
#ifdef RISCV_ISA_TESTS
    if (program_counter == 0x2003008) {
        int a = 0;
    }
    if (instruction == 0x00000073) {
        int a = 0;
    }
#endif
#endif
}
