//
// Created by hanyuan on 2024/2/8.
//
#include <stdio.h>
#include "mem.h"
#include "uart8250.h"

static uint32_t registers[32];
uint32_t program_counter = 0;

static uint8_t rom_ptr[ROM_SIZE];
static uint8_t ram_ptr[RAM_SIZE];

uint8_t *mem_get_ptr(uint32_t addr, int *ok_flag) {
    if (addr >= ROM_BASE_ADDR && addr < ROM_BASE_ADDR + ROM_SIZE) {
        *ok_flag = MEM_PTR_ROM;
        return &rom_ptr[addr - ROM_BASE_ADDR];
    } else if (addr >= RAM_BASE_ADDR && addr < RAM_BASE_ADDR + RAM_SIZE) {
        *ok_flag = MEM_PTR_RAM;
        return &ram_ptr[addr - RAM_BASE_ADDR];
    } else {
        *ok_flag = -1;
        return NULL;
    }
}

uint8_t mem_read_b(uint32_t addr, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }
    if (addr >= ROM_BASE_ADDR && addr < ROM_BASE_ADDR + ROM_SIZE) {
        return rom_ptr[addr - ROM_BASE_ADDR];
    } else if (addr >= RAM_BASE_ADDR && addr < RAM_BASE_ADDR + RAM_SIZE) {
        return ram_ptr[addr - RAM_BASE_ADDR];
    } else if (addr >= UART_BASE_ADDR && addr < UART_BASE_ADDR + UART_SIZE) {
        return uart8250_read_b(addr - UART_BASE_ADDR);
    } else {
        if (intr) {
            *intr = 1;
        }
        return 0xff;
    }
}

uint16_t mem_read_h(uint32_t addr, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }
    if (addr >= ROM_BASE_ADDR && addr + 1 < ROM_BASE_ADDR + ROM_SIZE) {
        return rom_ptr[addr - ROM_BASE_ADDR] | (rom_ptr[addr - ROM_BASE_ADDR + 1] << 8);
    } else if (addr >= RAM_BASE_ADDR && addr + 1 < RAM_BASE_ADDR + RAM_SIZE) {
        return ram_ptr[addr - RAM_BASE_ADDR] | (ram_ptr[addr - RAM_BASE_ADDR + 1] << 8);
    } else {
        if (intr) {
            *intr = 1;
        }
        return 0xffff;
    }
}

uint32_t mem_read_w(uint32_t addr, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }
    if (addr >= ROM_BASE_ADDR && addr + 3 < ROM_BASE_ADDR + ROM_SIZE) {
        return
                rom_ptr[addr - ROM_BASE_ADDR] |
                (rom_ptr[addr - ROM_BASE_ADDR + 1] << 8) |
                (rom_ptr[addr - ROM_BASE_ADDR + 2] << 16) |
                (rom_ptr[addr - ROM_BASE_ADDR + 3] << 24);
    } else if (addr >= RAM_BASE_ADDR && addr + 3 < RAM_BASE_ADDR + RAM_SIZE) {
        return
                ram_ptr[addr - RAM_BASE_ADDR] |
                (ram_ptr[addr - RAM_BASE_ADDR + 1] << 8) |
                (ram_ptr[addr - RAM_BASE_ADDR + 2] << 16) |
                (ram_ptr[addr - RAM_BASE_ADDR + 3] << 24);
    } else {
        if (intr) {
            *intr = 1;
        }

        return 0xffffffff;
    }
}

void mem_write_b(uint32_t addr, uint8_t data, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }
    if (addr >= RAM_BASE_ADDR && addr < RAM_BASE_ADDR + RAM_SIZE) {
        ram_ptr[addr - RAM_BASE_ADDR] = data;
    } else if (addr >= UART_BASE_ADDR && addr < UART_BASE_ADDR + UART_SIZE) {
        uart8250_write_b(addr - RAM_BASE_ADDR, data);
    } else {
        if (intr) {
            *intr = 1;
        }
    }
}

void mem_write_h(uint32_t addr, uint16_t data, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }
    if (addr >= RAM_BASE_ADDR && addr + 1 < RAM_BASE_ADDR + RAM_SIZE) {
        ram_ptr[addr - RAM_BASE_ADDR] = data & 0xff;
        ram_ptr[addr - RAM_BASE_ADDR + 1] = (data >> 8) & 0xff;
    } else {
        if (intr) {
            *intr = 1;
        }
    }
}

void mem_write_w(uint32_t addr, uint32_t data, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }
    if (addr >= RAM_BASE_ADDR && addr + 3 < RAM_BASE_ADDR + RAM_SIZE) {
        ram_ptr[addr - RAM_BASE_ADDR] = data & 0xff;
        ram_ptr[addr - RAM_BASE_ADDR + 1] = (data >> 8) & 0xff;
        ram_ptr[addr - RAM_BASE_ADDR + 2] = (data >> 16) & 0xff;
        ram_ptr[addr - RAM_BASE_ADDR + 3] = (data >> 24) & 0xff;
    } else {
        if (intr) {
            *intr = 1;
        }
    }
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
