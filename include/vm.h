//
// Created by hanyuan on 2024/2/20.
//

#ifndef TEMU_VM_H
#define TEMU_VM_H
#include "tlb.h"

#define PTE_V               0
#define PTE_R               1
#define PTE_W               2
#define PTE_X               3
#define PTE_U               4
#define PTE_G               5
#define PTE_A               6
#define PTE_D               7

#define PTE_MATCH(pte, flag)    ((pte >> flag) & 0x01)

#define SV32_ROOT(addr)         (addr << 12)
#define SV32_VOFFSET(addr)      (addr & 0x00000fff)
#define SV32_PTE2PA(pte)        ((pte >> 10) << 12)
#define SV32_VPN(addr, idx)     ((addr >> (idx==1 ? 22 : 12)) & 0x3ff)

uint8_t vm_on(void);

uint32_t vm_lookup_paddr(uint32_t vaddr, uint8_t *page_fault, uint8_t access_flags);

uint8_t pm_read_b(uint32_t addr, uint8_t *intr);

uint16_t pm_read_h(uint32_t addr, uint8_t *intr);

uint32_t pm_read_w(uint32_t addr, uint8_t *intr);

void pm_write_b(uint32_t addr, uint8_t data, uint8_t *intr);

void pm_write_h(uint32_t addr, uint16_t data, uint8_t *intr);

void pm_write_w(uint32_t addr, uint32_t data, uint8_t *intr);

#endif //TEMU_VM_H
