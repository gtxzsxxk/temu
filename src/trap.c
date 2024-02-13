//
// Created by hanyuan on 2024/2/11.
//
#include <stdint.h>
#include "mem.h"
#include "trap.h"
#include "zicsr.h"


void trap_throw_interrupt(uint32_t cause) {

}

void trap_throw_exception(uint32_t cause) {
    if (control_status_registers[CSR_idx_mstatus] & (1 << mstatus_MIE)) {
        /* set previous interrupt enable */
        control_status_registers[CSR_idx_mstatus] |= (1 << mstatus_MPIE);
        /* disable interrupt */
        control_status_registers[CSR_idx_mstatus] &= ~(1 << mstatus_MIE);
        control_status_registers[CSR_idx_mepc] = program_counter;
        program_counter = control_status_registers[CSR_idx_mtvec];
        if (control_status_registers[CSR_idx_mtvec] & 0x01) {
            /* Vectored */
            program_counter += 4 * cause;
        }
        control_status_registers[CSR_idx_mstatus] &= ~(3 << mstatus_MPP);
        control_status_registers[CSR_idx_mstatus] |= (current_privilege << mstatus_MPP);
        current_privilege = CSR_MASK_MACHINE;
    }
}