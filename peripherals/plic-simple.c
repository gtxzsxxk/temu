//
// Created by hanyuan on 2024/3/12.
//

#include "plic-simple.h"
#include "trap.h"

uint32_t pending_array_register_1 = 0;
uint32_t enables_hart_0_m_1 = 0;
uint32_t enables_hart_0_s_1 = 0;
uint32_t priority_threshold_hart_0_m = 0;
uint32_t claim_hart_0_m = 0;
uint32_t priority_threshold_hart_0_s = 0;
uint32_t claim_hart_0_s = 0;

uint8_t last_pending = INTERRUPT_MACHINE_EXTERNAL;

uint32_t plic_read_w(uint32_t offset) {
    switch (offset) {
        case 0x1000:
            return pending_array_register_1;
        case 0x2000:
            return enables_hart_0_m_1;
        case 0x2080:
            return enables_hart_0_s_1;
        case 0x200000:
            return priority_threshold_hart_0_m;
        case 0x200004:
            return claim_hart_0_m;
        case 0x201000:
            return priority_threshold_hart_0_s;
        case 0x201004:
            return claim_hart_0_s;
        default:
            return 0;
    }
}

void plic_write_w(uint32_t offset, uint32_t data) {
    switch (offset) {
        case 0x1000:
            pending_array_register_1 = data;
            break;
        case 0x2000:
            enables_hart_0_m_1 = data;
            break;
        case 0x2080:
            enables_hart_0_s_1 = data;
            break;
        case 0x200000:
            priority_threshold_hart_0_m = data;
            break;
        case 0x200004:
            if(claim_hart_0_m == data) {
                plic_clear_pending(claim_hart_0_m);
            }
            claim_hart_0_m = 0;
            break;
        case 0x201000:
            priority_threshold_hart_0_s = data;
            break;
        case 0x201004:
            if(claim_hart_0_s == data) {
                plic_clear_pending(claim_hart_0_s);
            }
            claim_hart_0_s = 0;
            break;
        default:
            break;
    }
}

void plic_throw_interrupt(uint32_t int_line) {
    if (enables_hart_0_m_1 & (1 << int_line)) {
        claim_hart_0_m = int_line;
        trap_throw_interrupt(INTERRUPT_MACHINE_EXTERNAL);
        last_pending = INTERRUPT_MACHINE_EXTERNAL;
    } else if (enables_hart_0_s_1 & (1 << int_line)) {
        claim_hart_0_s = int_line;
        trap_throw_interrupt(INTERRUPT_SUPERVISOR_EXTERNAL);
        last_pending = INTERRUPT_SUPERVISOR_EXTERNAL;
    }
    pending_array_register_1 |= (1 << int_line);
}

void plic_clear_pending(uint32_t int_line) {
    pending_array_register_1 &= ~(1 << int_line);
    if (pending_array_register_1 == 0) {
        trap_clear_interrupt_pending(last_pending);
    }
}
