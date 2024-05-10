//
// Created by hanyuan on 2024/2/8.
//
#include "mmu.h"
#include "tlb.h"
#include "cache.h"
#include "uart8250.h"
#include "plic-simple.h"
#include "port/main_memory.h"

#ifndef NULL
#define NULL (void*)0
#endif

static uint32_t registers[32];
uint32_t program_counter = 0;

static inline uint8_t vm_on(void) {
    return current_privilege <= CSR_MASK_SUPERVISOR && control_status_registers[CSR_idx_satp];
}

static inline uint8_t vm_status_read_mxr() {
    if (current_privilege == CSR_MASK_MACHINE) {
        return (control_status_registers[CSR_idx_mstatus] >> mstatus_MXR);
    }
    return (control_status_registers[CSR_idx_sstatus] >> mstatus_MXR);
}

static uint32_t vm_translation(uint32_t vaddr, uint8_t *page_fault, uint8_t *access_flags, uint8_t *user_only) {
    uint32_t pgtable = SV32_ROOT(control_status_registers[CSR_idx_satp]);
    uint32_t pte;
    uint8_t intr = 0;
    for (int i = 1; i >= 0; i--) {
        pte = cache_data_read_w(pgtable + SV32_VPN(vaddr, i) * 4, &intr);
        if (!PTE_MATCH(pte, PTE_V) || (!PTE_MATCH(pte, PTE_R) && PTE_MATCH(pte, PTE_W))) {
            if (page_fault) {
                *page_fault = 1;
            }
            return 0xabababab;
        }
        if (PTE_MATCH(pte, PTE_R) || PTE_MATCH(pte, PTE_X)) {
            /* leaf */
            if (!(pte & (1 << *access_flags)) ||
                (PTE_MATCH(pte, PTE_X) && !PTE_MATCH(pte, PTE_R) && (*access_flags == PTE_R) &&
                 !vm_status_read_mxr()) ||
                (vm_status_read_sum() == 0 && PTE_MATCH(pte, PTE_U) &&
                 current_privilege == CSR_MASK_SUPERVISOR) ||
                (i == 1 && (pte >> 10) & 0x3ff)) {
                if (page_fault) {
                    *page_fault = 1;
                }
                return 0xcdcdcdcd;
            }

            pte |= (1 << PTE_A);
            if (*access_flags | PTE_W) {
                pte |= (1 << PTE_D);
            }

            *user_only = PTE_MATCH(pte, PTE_U);
            *access_flags = (pte >> 1) & 0x07;
            /* the INTR shouldn't be 1 */
            cache_data_write_w(pgtable + SV32_VPN(vaddr, i) * 4, pte, NULL);
            uint32_t pa = SV32_PTE2PA(pte) + SV32_VOFFSET(vaddr);
            if (i == 1) {
                /* superpage */
                pa &= ~(0x3ff << 12);
                pa |= ((vaddr >> 12) & 0x3ff) << 12;
            }
            return pa;
        }
        pgtable = SV32_PTE2PA(pte);
    }

    if (page_fault) {
        *page_fault = 1;
    }
    return 0xefefefef;
}

uint32_t vm_lookup_paddr(uint32_t vaddr, uint8_t *page_fault, uint8_t access_flags) {
    uint8_t fault_flag;
    uint8_t user_only = 0;
    uint32_t ppn = tlb_lookup(vaddr, access_flags, &fault_flag);
    if (!fault_flag) {
        return (ppn << 12) | SV32_VOFFSET(vaddr);
    }
    if (fault_flag == TLB_PAGE_FAULT_IDENTIFIER) {
        *page_fault = 1;
    } else if (fault_flag == TLB_MISS_IDENTIFIER) {
        ppn = vm_translation(vaddr, page_fault, &access_flags, &user_only);
        if (!(*page_fault)) {
            struct tlb_cache_line line = {
                    .prot = access_flags,
                    .ppn = (ppn >> 12),
                    .user_only = user_only
            };
            tlb_insert(vaddr, line);
            return ppn;
        }
    }

    return 0x9a9a9a9a;
}

/* Used for debug only */
uint8_t pm_read_b(uint32_t addr, uint8_t *intr) {
    if (addr >= RAM_BASE_ADDR && addr < RAM_BASE_ADDR + RAM_SIZE) {
        uint8_t offset = addr & 0x03;
        return port_main_memory_read_w((addr & 0xfffffffc) - RAM_BASE_ADDR) >> offset;
    } else {
        if (intr) {
            *intr = 1;
        }
        return 0xff;
    }
}

uint8_t mmu_read_b(uint32_t addr, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_lookup_paddr(addr, &page_fault, PTE_R) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return 0xff;
    }

    if (addr_translated >= UART_BASE_ADDR && addr_translated < UART_BASE_ADDR + UART_SIZE) {
        return uart8250_read_b(addr_translated - UART_BASE_ADDR);
    }
    return cache_data_read_b(addr_translated, intr);
}

uint16_t mmu_read_h(uint32_t addr, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_lookup_paddr(addr, &page_fault, PTE_R) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return 0xffff;
    }

    return cache_data_read_h(addr_translated, intr);
}

uint32_t mmu_read_w(uint32_t addr, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_lookup_paddr(addr, &page_fault, PTE_R) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return 0x4444ffff;
    }

    if (addr_translated >= PLIC_BASE_ADDR && addr_translated < PLIC_BASE_ADDR + PLIC_SIZE) {
        return plic_read_w(addr_translated - PLIC_BASE_ADDR);
    }
    return cache_data_read_w(addr_translated, intr);
}

uint32_t mmu_read_inst(uint32_t addr, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_lookup_paddr(addr, &page_fault, PTE_X) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return 0x5555ffff;
    }

    return cache_inst_read(addr_translated, intr);
}

void mmu_write_b(uint32_t addr, uint8_t data, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_lookup_paddr(addr, &page_fault, PTE_W) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return;
    }

    if (addr_translated >= UART_BASE_ADDR && addr_translated < UART_BASE_ADDR + UART_SIZE) {
        uart8250_write_b(addr_translated - UART_BASE_ADDR, data);
        return;
    }

    cache_data_write_b(addr_translated, data, intr);
}

void mmu_write_h(uint32_t addr, uint16_t data, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_lookup_paddr(addr, &page_fault, PTE_W) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return;
    }

    cache_data_write_h(addr_translated, data, intr);
}

void mmu_write_w(uint32_t addr, uint32_t data, uint8_t *intr) {
    if (intr) {
        *intr = 0;
    }

    uint8_t page_fault = 0;
    uint32_t addr_translated = vm_on() ? vm_lookup_paddr(addr, &page_fault, PTE_W) : addr;
    if (page_fault) {
        if (intr) {
            *intr = 2;
        }
        return;
    }
    if (addr_translated >= PLIC_BASE_ADDR && addr_translated < PLIC_BASE_ADDR + PLIC_SIZE) {
        plic_write_w(addr_translated - PLIC_BASE_ADDR, data);
        return;
    }
    cache_data_write_w(addr_translated, data, intr);
}

uint32_t mmu_register_read(uint8_t rd) {
    if (rd == 0) {
        return 0;
    }
    return registers[rd];
}

void mmu_register_write(uint8_t rd, uint32_t value) {
    if (rd) {
        registers[rd] = value;
    }
}

#if TEMU_DEBUG_CODE
#include <stdio.h>

void mmu_debug_printreg(uint32_t pc_prev_exec) {
    printf("Emulator registers debug print\n");
    for (int i = 0; i < 32; i++) {
        printf("x%d: 0x%08x\n", i, mmu_register_read(i));
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

void mmu_debug_printaddr(uint32_t addr, uint8_t no_vaddr) {
    uint32_t addr_temp = addr;
    uint8_t (*read)(uint32_t, uint8_t *) = no_vaddr ? pm_read_b : mmu_read_b;
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

void mmu_debug_printstring(uint32_t addr, uint8_t no_vaddr) {
    uint32_t addr_temp = addr;
    uint8_t (*read)(uint32_t, uint8_t *) = no_vaddr ? pm_read_b : mmu_read_b;
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
#endif
