//
// Created by hanyuan on 2024/2/8.
//
#include <stdio.h>
#include "mem.h"
#include "uart8250.h"

static uint32_t registers[32];
uint32_t program_counter = 0;

uint8_t mem_read_b(uint32_t addr, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint32_t addr_translated = addr;

    if (addr_translated >= UART_BASE_ADDR && addr_translated < UART_BASE_ADDR + UART_SIZE) {
        return uart8250_read_b(addr_translated - UART_BASE_ADDR);
    }
    return pm_read_b(addr_translated, intr);
}

uint16_t mem_read_h(uint32_t addr, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint32_t addr_translated = addr;

    return pm_read_h(addr_translated, intr);
}

uint32_t mem_read_w(uint32_t addr, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint32_t addr_translated = addr;

    return pm_read_w(addr_translated, intr);
}

void mem_write_b(uint32_t addr, uint8_t data, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint32_t addr_translated = addr;

    if (addr_translated >= UART_BASE_ADDR && addr_translated < UART_BASE_ADDR + UART_SIZE) {
        uart8250_write_b(addr_translated - RAM_BASE_ADDR, data);
        return;
    }

    pm_write_b(addr_translated, data, intr);
}

void mem_write_h(uint32_t addr, uint16_t data, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint32_t addr_translated = addr;

    pm_write_h(addr_translated, data, intr);
}

void mem_write_w(uint32_t addr, uint32_t data, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint32_t addr_translated = addr;

    pm_write_w(addr_translated, data, intr);
}

uint32_t mem_register_read(uint8_t rd) {
    if (rd == 0) {
        return 0;
    }
    return registers[rd];
}

void mem_register_write(uint8_t rd, uint32_t value) {
    if (rd) {
        registers[rd] = value;
    }
}

void mem_debug_printreg(uint32_t pc_prev_exec) {
    printf("Emulator registers debug print\n");
    for (int i = 0; i < 32; i++) {
        printf("x%d: 0x%08x\n", i, mem_register_read(i));
    }
    printf("PC: 0x%08x\n\n", pc_prev_exec);
    fflush(stdout);
}
