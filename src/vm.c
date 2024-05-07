//
// Created by hanyuan on 2024/2/20.
//
#include <stdio.h>
#include <stdint.h>
#include <parameters.h>
#include "vm.h"
#include "zicsr.h"
#include "port/main_memory.h"

uint8_t vm_on(void) {
    return current_privilege <= CSR_MASK_SUPERVISOR && control_status_registers[CSR_idx_satp];
}

static inline uint8_t vm_status_read_mxr() {
    if (current_privilege == CSR_MASK_MACHINE) {
        return (control_status_registers[CSR_idx_mstatus] >> mstatus_MXR);
    }
    return (control_status_registers[CSR_idx_sstatus] >> mstatus_MXR);
}

uint8_t vm_status_read_sum() {
    if (current_privilege == CSR_MASK_MACHINE) {
        return (control_status_registers[CSR_idx_mstatus] >> mstatus_SUM);
    }
    return (control_status_registers[CSR_idx_sstatus] >> mstatus_SUM);
}

uint32_t vm_translation(uint32_t vaddr, uint8_t *page_fault, uint8_t access_flags) {
    uint32_t pgtable = SV32_ROOT(control_status_registers[CSR_idx_satp]);
    uint32_t pte;
    uint8_t intr;
    for (int i = 1; i >= 0; i--) {
        pte = pm_read_w(pgtable + SV32_VPN(vaddr, i) * 4, &intr);
        if (!PTE_MATCH(pte, PTE_V) || (!PTE_MATCH(pte, PTE_R) && PTE_MATCH(pte, PTE_W))) {
            if (page_fault) {
                *page_fault = 1;
            }
            return 0xabababab;
        }
        if (PTE_MATCH(pte, PTE_R) || PTE_MATCH(pte, PTE_X)) {
            /* leaf */
            if ((pte & (1 << access_flags)) != (1 << access_flags) ||
                (PTE_MATCH(pte, PTE_X) && !PTE_MATCH(pte, PTE_R) && (access_flags == PTE_R) &&
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
            if (access_flags | PTE_W) {
                pte |= (1 << PTE_D);
            }
            /* the INTR shouldn't be 1 */
            pm_write_w(pgtable + SV32_VPN(vaddr, i) * 4, pte, NULL);
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

uint8_t pm_read_b(uint32_t addr, uint8_t *intr) {
    if (addr >= RAM_BASE_ADDR && addr < RAM_BASE_ADDR + RAM_SIZE) {
        return port_main_memory_read(addr - RAM_BASE_ADDR);
    } else {
        if (intr) {
            *intr = 1;
        }
        return 0xff;
    }
}

uint16_t pm_read_h(uint32_t addr, uint8_t *intr) {
    if (addr % 2) {
        if (intr) {
            *intr = 3;
        }
        return 0xff;
    }
    if (addr >= RAM_BASE_ADDR && addr + 1 < RAM_BASE_ADDR + RAM_SIZE) {
        return port_main_memory_read(addr - RAM_BASE_ADDR) | (port_main_memory_read(addr - RAM_BASE_ADDR + 1) << 8);
    } else {
        if (intr) {
            *intr = 1;
        }
        return 0xffff;
    }
}

uint32_t pm_read_w(uint32_t addr, uint8_t *intr) {
    if (addr % 4) {
        if (intr) {
            *intr = 3;
        }
        return 0xff;
    }
    if (addr >= RAM_BASE_ADDR && addr + 3 < RAM_BASE_ADDR + RAM_SIZE) {
        return
                port_main_memory_read(addr - RAM_BASE_ADDR) |
                (port_main_memory_read(addr - RAM_BASE_ADDR + 1) << 8) |
                (port_main_memory_read(addr - RAM_BASE_ADDR + 2) << 16) |
                (port_main_memory_read(addr - RAM_BASE_ADDR + 3) << 24);
    } else {
        if (intr) {
            *intr = 1;
        }

        return 0x6666ffff;
    }
}

void pm_write_b(uint32_t addr, uint8_t data, uint8_t *intr) {
    if (addr >= RAM_BASE_ADDR && addr < RAM_BASE_ADDR + RAM_SIZE) {
        port_main_memory_write(addr - RAM_BASE_ADDR, data);
    } else {
        if (intr) {
            *intr = 1;
        }
    }
}

void pm_write_h(uint32_t addr, uint16_t data, uint8_t *intr) {
    if (addr % 2) {
        if (intr) {
            *intr = 3;
        }
    }
    if (addr >= RAM_BASE_ADDR && addr + 1 < RAM_BASE_ADDR + RAM_SIZE) {
        port_main_memory_write(addr - RAM_BASE_ADDR, data & 0xff);
        port_main_memory_write(addr - RAM_BASE_ADDR + 1, (data >> 8) & 0xff);
    } else {
        if (intr) {
            *intr = 1;
        }
    }
}

void pm_write_w(uint32_t addr, uint32_t data, uint8_t *intr) {
    if (addr % 4) {
        if (intr) {
            *intr = 3;
        }
    }
    if (addr >= RAM_BASE_ADDR && addr + 3 < RAM_BASE_ADDR + RAM_SIZE) {
        port_main_memory_write(addr - RAM_BASE_ADDR, data & 0xff);
        port_main_memory_write(addr - RAM_BASE_ADDR + 1, (data >> 8) & 0xff);
        port_main_memory_write(addr - RAM_BASE_ADDR + 2, (data >> 16) & 0xff);
        port_main_memory_write(addr - RAM_BASE_ADDR + 3, (data >> 24) & 0xff);
    } else {
        if (intr) {
            *intr = 1;
        }
    }
}