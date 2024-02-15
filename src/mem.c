//
// Created by hanyuan on 2024/2/8.
//
#include "mem.h"

static uint32_t registers[32];
uint32_t program_counter = 0;

static uint8_t rom_ptr[ROM_SIZE];
static uint8_t ram_ptr[RAM_SIZE];

uint8_t *mem_get_rom_ptr() {
    return rom_ptr;
}

uint8_t mem_read_b(uint32_t addr) {
    if (addr >= ROM_START_ADDR && addr < ROM_START_ADDR + ROM_SIZE) {
        return rom_ptr[addr - ROM_START_ADDR];
    } else if (addr >= RAM_START_ADDR && addr < RAM_START_ADDR + RAM_SIZE) {
        return ram_ptr[addr - RAM_START_ADDR];
    } else {
        /* Illegal memory access interrupt */
        return 0xff;
    }
}

uint16_t mem_read_h(uint32_t addr) {
    if (addr >= ROM_START_ADDR && addr + 2 < ROM_START_ADDR + ROM_SIZE) {
        return rom_ptr[addr - ROM_START_ADDR] | (rom_ptr[addr - ROM_START_ADDR + 1] << 8);
    } else if (addr >= RAM_START_ADDR && addr +2 < RAM_START_ADDR + RAM_SIZE) {
        return ram_ptr[addr - RAM_START_ADDR] | (ram_ptr[addr - RAM_START_ADDR + 1] << 8);
    } else {
        /* Illegal memory access interrupt */
        return 0xffff;
    }
}

uint32_t mem_read_w(uint32_t addr) {
    if (addr >= ROM_START_ADDR && addr + 4 < ROM_START_ADDR + ROM_SIZE) {
        return
                rom_ptr[addr - ROM_START_ADDR] |
                (rom_ptr[addr - ROM_START_ADDR + 1] << 8) |
                (rom_ptr[addr - ROM_START_ADDR + 2] << 16) |
                (rom_ptr[addr - ROM_START_ADDR + 3] << 24);
    } else if (addr >= RAM_START_ADDR && addr + 4 < RAM_START_ADDR + RAM_SIZE) {
        return
                ram_ptr[addr - RAM_START_ADDR] |
                (ram_ptr[addr - RAM_START_ADDR + 1] << 8) |
                (ram_ptr[addr - RAM_START_ADDR + 2] << 16) |
                (ram_ptr[addr - RAM_START_ADDR + 3] << 24);
    } else {
        /* Illegal memory access interrupt */
        return 0xffffffff;
    }
}

void mem_write_b(uint32_t addr, uint8_t data) {
    if (addr >= ROM_START_ADDR && addr < ROM_START_ADDR + ROM_SIZE) {
        rom_ptr[addr - ROM_START_ADDR] = data;
    } else if (addr >= RAM_START_ADDR && addr < RAM_START_ADDR + RAM_SIZE) {
        ram_ptr[addr - RAM_START_ADDR] = data;
    } else {
        /* Illegal memory access interrupt */
    }
}

void mem_write_h(uint32_t addr, uint16_t data) {
    if (addr >= ROM_START_ADDR && addr + 2 < ROM_START_ADDR + ROM_SIZE) {
        rom_ptr[addr - ROM_START_ADDR] = data & 0xff;
        rom_ptr[addr - ROM_START_ADDR + 1] = (data >> 8) & 0xff;
    } else if (addr >= RAM_START_ADDR && addr +2 < RAM_START_ADDR + RAM_SIZE) {
        ram_ptr[addr - RAM_START_ADDR] = data & 0xff;
        ram_ptr[addr - RAM_START_ADDR + 1] = (data >> 8) & 0xff;
    } else {
        /* Illegal memory access interrupt */
    }
}

void mem_write_w(uint32_t addr, uint32_t data) {
    if (addr >= ROM_START_ADDR && addr + 4 < ROM_START_ADDR + ROM_SIZE) {
        rom_ptr[addr - ROM_START_ADDR] = data & 0xff;
        rom_ptr[addr - ROM_START_ADDR + 1] = (data >> 8) & 0xff;
        rom_ptr[addr - ROM_START_ADDR + 2] = (data >> 16) & 0xff;
        rom_ptr[addr - ROM_START_ADDR + 3] = (data >> 24) & 0xff;
    } else if (addr >= RAM_START_ADDR && addr + 4 < RAM_START_ADDR + RAM_SIZE) {
        ram_ptr[addr - RAM_START_ADDR] = data & 0xff;
        ram_ptr[addr - RAM_START_ADDR + 1] = (data >> 8) & 0xff;
        ram_ptr[addr - RAM_START_ADDR + 2] = (data >> 16) & 0xff;
        ram_ptr[addr - RAM_START_ADDR + 3] = (data >> 24) & 0xff;
    } else {
        /* Illegal memory access interrupt */
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
