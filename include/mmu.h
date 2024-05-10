//
// Created by hanyuan on 2024/2/8.
//

#ifndef TEMU_MMU_H
#define TEMU_MMU_H

#include <stdint.h>
#include "parameters.h"
#include "zicsr.h"

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

extern uint32_t program_counter;

uint8_t mmu_read_b(uint32_t addr, uint8_t *intr);

uint16_t mmu_read_h(uint32_t addr, uint8_t *intr);

uint32_t mmu_read_w(uint32_t addr, uint8_t *intr);

uint32_t mmu_read_inst(uint32_t addr, uint8_t *intr);

void mmu_write_b(uint32_t addr, uint8_t data, uint8_t *intr);

void mmu_write_h(uint32_t addr, uint16_t data, uint8_t *intr);

void mmu_write_w(uint32_t addr, uint32_t data, uint8_t *intr);

uint32_t mmu_register_read(uint8_t rd);

void mmu_register_write(uint8_t rd, uint32_t value);

void mmu_debug_printreg(uint32_t pc_prev_exec);

#if TEMU_DEBUG_CODE
void mmu_debug_printaddr(uint32_t addr, uint8_t no_vaddr);

void mmu_debug_printstring(uint32_t addr, uint8_t no_vaddr);
#endif

#endif //TEMU_MMU_H

static inline uint8_t vm_status_read_sum() {
    if (current_privilege == CSR_MASK_MACHINE) {
        return (control_status_registers[CSR_idx_mstatus] >> mstatus_SUM);
    }
    return (control_status_registers[CSR_idx_sstatus] >> mstatus_SUM);
}
