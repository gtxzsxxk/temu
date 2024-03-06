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

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_translation(addr, &page_fault, PTE_R) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return 0xff;
    }

    if (addr_translated >= UART_BASE_ADDR && addr_translated < UART_BASE_ADDR + UART_SIZE) {
        return uart8250_read_b(addr_translated - UART_BASE_ADDR);
    }
    return pm_read_b(addr_translated, intr);
}

uint16_t mem_read_h(uint32_t addr, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_translation(addr, &page_fault, PTE_R) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return 0xffff;
    }

    return pm_read_h(addr_translated, intr);
}

uint32_t mem_read_w(uint32_t addr, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_translation(addr, &page_fault, PTE_R) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return 0xffffffff;
    }

    return pm_read_w(addr_translated, intr);
}

void mem_write_b(uint32_t addr, uint8_t data, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_translation(addr, &page_fault, PTE_W) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return;
    }

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

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_translation(addr, &page_fault, PTE_W) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return;
    }

    pm_write_h(addr_translated, data, intr);
}

void mem_write_w(uint32_t addr, uint32_t data, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_translation(addr, &page_fault, PTE_W) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return;
    }

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

static uint8_t __inline__ char_print(uint8_t data) {
    if (data >= 0x20 && data <= 0x7E) {
        return data;
    }
    return '.';
}

void mem_debug_printaddr(uint32_t addr, uint8_t no_vaddr) {
    uint32_t addr_temp = addr;
    uint8_t (*read)(uint32_t, uint8_t *) = no_vaddr ? pm_read_b : mem_read_b;
    for (int i = 0; i < 256; i++) {
        printf("0x%08x:\t\t%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\t\t%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
               addr_temp,
               read(addr_temp + 0, NULL),
               read(addr_temp + 1, NULL),
               read(addr_temp + 2, NULL),
               read(addr_temp + 3, NULL),
               read(addr_temp + 4, NULL),
               read(addr_temp + 5, NULL),
               read(addr_temp + 6, NULL),
               read(addr_temp + 7, NULL),
               read(addr_temp + 8, NULL),
               read(addr_temp + 9, NULL),
               read(addr_temp + 10, NULL),
               read(addr_temp + 11, NULL),
               read(addr_temp + 12, NULL),
               read(addr_temp + 13, NULL),
               read(addr_temp + 14, NULL),
               read(addr_temp + 15, NULL),
               char_print(read(addr_temp + 0, NULL)),
               char_print(read(addr_temp + 1, NULL)),
               char_print(read(addr_temp + 2, NULL)),
               char_print(read(addr_temp + 3, NULL)),
               char_print(read(addr_temp + 4, NULL)),
               char_print(read(addr_temp + 5, NULL)),
               char_print(read(addr_temp + 6, NULL)),
               char_print(read(addr_temp + 7, NULL)),
               char_print(read(addr_temp + 8, NULL)),
               char_print(read(addr_temp + 9, NULL)),
               char_print(read(addr_temp + 10, NULL)),
               char_print(read(addr_temp + 11, NULL)),
               char_print(read(addr_temp + 12, NULL)),
               char_print(read(addr_temp + 13, NULL)),
               char_print(read(addr_temp + 14, NULL)),
               char_print(read(addr_temp + 15, NULL))
        );
        addr_temp += 16;
    }
    fflush(stdout);
}

void mem_debug_printstring(uint32_t addr, uint8_t no_vaddr) {
    uint32_t addr_temp = addr;
    uint8_t (*read)(uint32_t, uint8_t *) = no_vaddr ? pm_read_b : mem_read_b;
    printf("\n===debug string print @ %08x\n", addr);
    for (int i = 0; i < 256; i++) {
        uint8_t data = read(addr_temp + i, NULL);
        if (!data) {
            break;
        }
        printf("%c", data);
    }
    printf("\n===end of debug string print\n");
    fflush(stdout);
}
