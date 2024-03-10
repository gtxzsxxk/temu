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

static uint8_t access_error_intr;

static void machine_pre_boot(uint32_t start);

static void machine_tick(void);

static void machine_debug(uint32_t instruction, int printreg);

static void set_terminal(void);

void digest(uint32_t addr, uint32_t len);

_Noreturn void machine_start(uint32_t start, int printreg) {
    uint32_t dynamic_debug_pc = 0;
    machine_pre_boot(start);

    for (;;) {
        access_error_intr = 0;
        if (program_counter == 0xc03ee534) {
            /* register earlycon in setup_earlycon*/
            int a = 0;
        }
        if (program_counter == 0xC004F09C) {
            /* con->write */
            int a = 0;
        }
        if (program_counter == 0xc03de110) {
            /* paging init's printk */
            int a = 0;
        }
        if (program_counter == 0xc004ef04) {
            /* entry of console_flush_all */
            int a = 0;
        }
        if (program_counter == dynamic_debug_pc) {
            int a = 0;
        }
        if (program_counter == 0xc03de2d8) {
            /* for each mem */
            int a = 0;
        }
        if (program_counter == 0xc03dddec) {
            /* alloc_pgd_next */
            int a = 0;
        }
        if (program_counter == 0xc004d804) {
            /* vprintk_store */
            int a = 0;
        }
        if (program_counter == 0xc03bb400) {
            /* panic */
            int a = 0;
        }
        if (program_counter == 0xc004dda4) {
            /* printk_get_next_message */
            int a = 0;
        }
        if (program_counter == 0xc004dbd8) {
            /* vprintk_store printk_sprint*/
            int a = 0;
        }

        uint32_t instruction = mem_read_w(program_counter, &access_error_intr);
        if (access_error_intr) {
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

#include <stdio.h>
#include <stdlib.h>

void digest(uint32_t addr, uint32_t len) {
    FILE *fp = fopen("out.bin","w");

    for(uint32_t i = addr; i < addr + len;i++){
        uint8_t dat = pm_read_b(i,NULL);
        fwrite(&dat,1,1,fp);
    }
    fclose(fp);
}

static void machine_pre_boot(uint32_t start) {
    program_counter = start;

    set_terminal();
    uart8250_init();
}

static void machine_tick(void) {
    uart8250_tick();
    zicnt_cycle_tick();

    if (zicnt_get_cycle() % (SIM_YIELD_GAP / 10000) == 0) {
        zicnt_time_tick();
#if !SIM_FULL_SPEED
        usleep(SIM_YIELD_TIME);
#endif
    }

    trap_take_interrupt();
}

static void machine_debug(uint32_t instruction, int printreg) {
#ifdef RISCV_DEBUG
    if (printreg) {
        mem_debug_printreg(program_counter);
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

static void set_terminal(void) {
    static struct termios tm;
    tcgetattr(STDIN_FILENO, &tm);
    cfmakeraw(&tm);
    tm.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &tm);
}
