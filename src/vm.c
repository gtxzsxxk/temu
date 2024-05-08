//
// Created by hanyuan on 2024/2/20.
//
#include <stdint.h>
#include <parameters.h>
#include "vm.h"
#include "cache.h"
#include "zicsr.h"
#include "port/main_memory.h"

#ifndef NULL
#define NULL (void*)0
#endif

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
