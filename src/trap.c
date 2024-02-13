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

void trap_return_machine(void) {
    current_privilege = (control_status_registers[CSR_idx_mstatus] >> mstatus_MPP) & 0x03;
    if (control_status_registers[CSR_idx_mstatus] & (1 << mstatus_MPIE)) {
        control_status_registers[CSR_idx_mstatus] |= (1 << mstatus_MIE);
    } else {
        control_status_registers[CSR_idx_mstatus] &= ~(1 << mstatus_MIE);
    }
    program_counter = control_status_registers[CSR_idx_mepc];
}

void trap_return_supervisor(void) {
    current_privilege = (control_status_registers[CSR_idx_sstatus] >> mstatus_SPP);
    if (control_status_registers[CSR_idx_sstatus] & (1 << sstatus_SPIE)) {
        control_status_registers[CSR_idx_sstatus] |= (1 << sstatus_SIE);
    } else {
        control_status_registers[CSR_idx_sstatus] &= ~(1 << sstatus_SIE);
    }
    program_counter = control_status_registers[CSR_idx_sepc];
}