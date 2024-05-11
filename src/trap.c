//
// Created by hanyuan on 2024/2/11.
//
#include <stdint.h>
#include "mmu.h"
#include "trap.h"
#include "zicsr.h"

uint8_t in_wfi = 0;
uint32_t wfi_next_pc = 0;

static uint32_t trap_get_program_counter(void) {
    if (in_wfi) {
        in_wfi = 0;
        return wfi_next_pc;
    }
    return program_counter;
}

uint32_t trap_is_pending(void) {
    return control_status_registers[CSR_idx_sip];
}

void trap_clear_interrupt_pending(uint32_t cause) {
    control_status_registers[CSR_idx_sip] &= ~(1 << cause);
    control_status_registers[CSR_idx_mip] &= ~(1 << cause);
}

void trap_throw_interrupt(uint32_t cause) {
    if ((1 << cause) & control_status_registers[CSR_idx_mideleg]) {
        /* delegate to supervisor mode */
        control_status_registers[CSR_idx_sip] |= (1 << cause);
    } else {
        control_status_registers[CSR_idx_mip] |= (1 << cause);
    }
}

void trap_take_interrupt(void) {
    uint32_t cause = 0;
    /* TODO: priority */
    uint32_t pending = trap_is_pending();
    if (!trap_is_pending()) {
        return;
    }
    while (!(pending & 0x01)) {
        cause++;
        pending = pending >> 1;
    }
    if (!cause) {
        return;
    }
    if ((1 << cause) & control_status_registers[CSR_idx_mideleg]) {
        /* delegate to supervisor mode */
        if (!(control_status_registers[CSR_idx_sstatus] & (1 << mstatus_SIE)) && current_privilege > CSR_MASK_USER &&
            !in_wfi) {
            return;
        }
        if (!(control_status_registers[CSR_idx_sie] & (1 << cause))) {
            return;
        }
        /* set cause */
        control_status_registers[CSR_idx_scause] = cause | 0x80000000;
        control_status_registers[CSR_idx_stval] = 0;
        /* set previous interrupt enable */
        control_status_registers[CSR_idx_sstatus] |= (1 << mstatus_SPIE);
        /* disable interrupt */
        control_status_registers[CSR_idx_sstatus] &= ~(1 << mstatus_SIE);

        control_status_registers[CSR_idx_sepc] = trap_get_program_counter();
        program_counter = control_status_registers[CSR_idx_stvec];
        if (control_status_registers[CSR_idx_stvec] & 0x01) {
            /* Vectored */
            program_counter += 4 * cause;
        }

        /* Set SPP only */
        if (current_privilege == CSR_MASK_USER) {
            control_status_registers[CSR_idx_sstatus] &= ~(1 << mstatus_SPP);
        }
        if (current_privilege > CSR_MASK_USER) {
            control_status_registers[CSR_idx_sstatus] |= ((current_privilege & 0x01) << mstatus_SPP);
        }

        current_privilege = CSR_MASK_SUPERVISOR;
    } else {
        if (!(control_status_registers[CSR_idx_mstatus] & (1 << mstatus_MIE)) &&
            current_privilege == CSR_MASK_MACHINE && !in_wfi) {
            return;
        }
        if (!(control_status_registers[CSR_idx_mie] & (1 << cause))) {
            return;
        }
        /* set cause */
        control_status_registers[CSR_idx_mcause] = cause | 0x80000000;
        control_status_registers[CSR_idx_mtval] = 0;
        /* set previous interrupt enable */
        control_status_registers[CSR_idx_mstatus] |= (1 << mstatus_MPIE);
        /* disable interrupt */
        control_status_registers[CSR_idx_mstatus] &= ~(1 << mstatus_MIE);

        control_status_registers[CSR_idx_mepc] = trap_get_program_counter();
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

void trap_throw_exception(uint32_t cause, uint32_t tval) {
    if ((1 << cause) & control_status_registers[CSR_idx_medeleg]) {
        /* delegate to supervisor mode */
        /* set cause */
        control_status_registers[CSR_idx_scause] = cause;
        control_status_registers[CSR_idx_stval] = tval;
        control_status_registers[CSR_idx_scause] &= 0x7fffffff;
        /* set previous interrupt enable */
        control_status_registers[CSR_idx_sstatus] |= (1 << mstatus_SPIE);
        /* disable interrupt */
        control_status_registers[CSR_idx_sstatus] &= ~(1 << mstatus_SIE);

        control_status_registers[CSR_idx_sepc] = trap_get_program_counter();
        program_counter = control_status_registers[CSR_idx_stvec];
        if (control_status_registers[CSR_idx_stvec] & 0x01) {
            /* Vectored */
            program_counter += 4 * cause;
        }

        /* Set SPP only */
        if (current_privilege == CSR_MASK_USER) {
            control_status_registers[CSR_idx_sstatus] &= ~(1 << mstatus_SPP);
        }
        if (current_privilege > CSR_MASK_USER) {
            control_status_registers[CSR_idx_sstatus] |= ((current_privilege & 0x01) << mstatus_SPP);
        }

        current_privilege = CSR_MASK_SUPERVISOR;
    } else {
        /* set cause */
        control_status_registers[CSR_idx_mcause] = cause;
        control_status_registers[CSR_idx_mtval] = tval;
        control_status_registers[CSR_idx_mcause] &= 0x7fffffff;
        /* set previous interrupt enable */
        control_status_registers[CSR_idx_mstatus] |= (1 << mstatus_MPIE);
        /* disable interrupt */
        control_status_registers[CSR_idx_mstatus] &= ~(1 << mstatus_MIE);

        control_status_registers[CSR_idx_mepc] = trap_get_program_counter();
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
    control_status_registers[CSR_idx_mstatus] &= ~(0x03 << mstatus_MPP);
    if (control_status_registers[CSR_idx_mstatus] & (1 << mstatus_MPIE)) {
        control_status_registers[CSR_idx_mstatus] |= (1 << mstatus_MIE);
    } else {
        control_status_registers[CSR_idx_mstatus] &= ~(1 << mstatus_MIE);
    }
    control_status_registers[CSR_idx_mstatus] |= (1 << mstatus_MPIE);
    program_counter = control_status_registers[CSR_idx_mepc];
}

void trap_return_supervisor(void) {
    current_privilege = (control_status_registers[CSR_idx_sstatus] >> mstatus_SPP);
    control_status_registers[CSR_idx_sstatus] &= ~(1 << mstatus_SPP);
    if (control_status_registers[CSR_idx_sstatus] & (1 << sstatus_SPIE)) {
        control_status_registers[CSR_idx_sstatus] |= (1 << sstatus_SIE);
    } else {
        control_status_registers[CSR_idx_sstatus] &= ~(1 << sstatus_SIE);
    }
    control_status_registers[CSR_idx_sstatus] |= (1 << sstatus_SPIE);
    program_counter = control_status_registers[CSR_idx_sepc];
}
