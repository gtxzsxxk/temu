//
// Created by hanyuan on 2024/2/10.
//

/* TODO: use port to provide timer information */
#include <time.h>
#include "mem.h"
#include "trap.h"
#include "zicsr.h"

uint8_t current_privilege = CSR_MASK_MACHINE;

static uint64_t cycle = 0;
static uint64_t csr_time = 0;
static uint64_t timecmp = 0xffffffffffffffff;

uint32_t control_status_registers[CSR_SIZE] = {
        CSR_RESET_VALUE(misa, 0x40140101)
        CSR_RESET_VALUE(mvendorid, 0x00000000)
        CSR_RESET_VALUE(mhartid, 0x00000000)
        CSR_RESET_VALUE(cycle, 0x00000000)
        CSR_RESET_VALUE(cycleh, 0x00000000)
        CSR_RESET_VALUE(time, 0x00000000)
        CSR_RESET_VALUE(timeh, 0x00000000)
        CSR_RESET_VALUE(stimecmp, 0x00000000)
        CSR_RESET_VALUE(stimecmph, 0x00000000)
};
const uint32_t csr_match[CSR_SIZE] = {
        CSR_MATCH_DECLARE(sstatus)
        CSR_MATCH_DECLARE(sie)
        CSR_MATCH_DECLARE(stvec)
        CSR_MATCH_DECLARE(scounteren)
        CSR_MATCH_DECLARE(senvcfg)
        CSR_MATCH_DECLARE(sscratch)
        CSR_MATCH_DECLARE(sepc)
        CSR_MATCH_DECLARE(scause)
        CSR_MATCH_DECLARE(stval)
        CSR_MATCH_DECLARE(sip)
        CSR_MATCH_DECLARE(stimecmp)
        CSR_MATCH_DECLARE(stimecmph)
        CSR_MATCH_DECLARE(satp)
        CSR_MATCH_DECLARE(mstatus)
        CSR_MATCH_DECLARE(misa)
        CSR_MATCH_DECLARE(medeleg)
        CSR_MATCH_DECLARE(mideleg)
        CSR_MATCH_DECLARE(mie)
        CSR_MATCH_DECLARE(mtvec)
        CSR_MATCH_DECLARE(mcounteren)
        CSR_MATCH_DECLARE(mstatush)
        CSR_MATCH_DECLARE(mcountinhibit)
        CSR_MATCH_DECLARE(mscratch)
        CSR_MATCH_DECLARE(mepc)
        CSR_MATCH_DECLARE(mcause)
        CSR_MATCH_DECLARE(mtval)
        CSR_MATCH_DECLARE(mip)
        CSR_MATCH_DECLARE(mtinst)
        CSR_MATCH_DECLARE(scontext)
        CSR_MATCH_DECLARE(cycle)
        CSR_MATCH_DECLARE(time)
        CSR_MATCH_DECLARE(cycleh)
        CSR_MATCH_DECLARE(timeh)
        CSR_MATCH_DECLARE(mvendorid)
        CSR_MATCH_DECLARE(marchid)
        CSR_MATCH_DECLARE(mimpid)
        CSR_MATCH_DECLARE(mhartid)
        CSR_MATCH_DECLARE(mconfigptr)
        CSR_MATCH_DECLARE(menvcfg)
        CSR_MATCH_DECLARE(menvcfgh)
};
const uint8_t csr_index_remap[0x10] = {
        [0x0] = 100,
        [0x2] = 100,
        [0x4] = 100,
        [0x6] = 100,
        [0x7] = 100,
        [0x8] = 100,
        [0x9] = 100,
        [0xA] = 100,
        [0xB] = 100,
        [0xD] = 100,
        [0xE] = 100,

        [0x1] = CSR_idx_sstatus,
        [0x3] = CSR_idx_mstatus,
        [0x5] = CSR_idx_scontext,
        [0xC] = CSR_idx_cycle,
        [0xF] = CSR_idx_mvendorid,
};

uint8_t csr_get_index_by_number(uint16_t csr_number) {
    uint8_t start_index = csr_index_remap[csr_number >> 8];
    if (start_index == 100) {
        return -1;
    }
    for (; start_index < CSR_SIZE; start_index++) {
        if (((csr_match[start_index] >> 8) & 0xFFF) == csr_number) {
            return start_index;
        }
    }
    return -1;
}

uint32_t csr_read(uint16_t csr_index) {
    /* TODO: implement read actions */
    return control_status_registers[csr_index];
}

void csr_write(uint16_t csr_index, uint32_t value) {
    /* TODO: implement write actions */
    if (csr_match[csr_index] & CSR_MASK_WRITE) {
        control_status_registers[csr_index] = value;
    }
}

void csr_csrrw(uint8_t rs1, uint8_t rd, uint16_t csr_number, uint8_t *intr) {
    uint8_t index = csr_get_index_by_number(csr_number);
    *intr = 0;
    if (index == (uint8_t) -1) {
        *intr = 1;
        return;
    }
    if (rd) {
        uint32_t prev_value = csr_read(index);
        csr_write(index, mem_register_read(rs1));
        mem_register_write(rd, prev_value);
    } else {
        csr_write(index, mem_register_read(rs1));
    }
}

void csr_csrrs(uint8_t rs1, uint8_t rd, uint16_t csr_number, uint8_t *intr) {
    uint8_t index = csr_get_index_by_number(csr_number);
    *intr = 0;
    if (index == (uint8_t) -1) {
        *intr = 1;
        return;
    }
    uint32_t prev_value = csr_read(index);
    if (rs1) {
        csr_write(index, prev_value | mem_register_read(rs1));
    }
    mem_register_write(rd, prev_value);
}

void csr_csrrc(uint8_t rs1, uint8_t rd, uint16_t csr_number, uint8_t *intr) {
    uint8_t index = csr_get_index_by_number(csr_number);
    *intr = 0;
    if (index == (uint8_t) -1) {
        *intr = 1;
        return;
    }
    uint32_t prev_value = csr_read(index);
    if (rs1) {
        csr_write(index, prev_value & (~mem_register_read(rs1)));
    }
    mem_register_write(rd, prev_value);
}

void csr_csrrwi(uint8_t uimm, uint8_t rd, uint16_t csr_number, uint8_t *intr) {
    uint8_t index = csr_get_index_by_number(csr_number);
    *intr = 0;
    if (index == (uint8_t) -1) {
        *intr = 1;
        return;
    }
    if (rd) {
        uint32_t prev_value = csr_read(index);
        csr_write(index, uimm);
        mem_register_write(rd, prev_value);
    } else {
        csr_write(index, uimm);
    }
}

void csr_csrrsi(uint8_t uimm, uint8_t rd, uint16_t csr_number, uint8_t *intr) {
    uint8_t index = csr_get_index_by_number(csr_number);
    *intr = 0;
    if (index == (uint8_t) -1) {
        *intr = 1;
        return;
    }
    uint32_t prev_value = csr_read(index);
    mem_register_write(rd, prev_value);
    if (uimm) {
        csr_write(index, prev_value | uimm);
    }
}

void csr_csrrci(uint8_t uimm, uint8_t rd, uint16_t csr_number, uint8_t *intr) {
    uint8_t index = csr_get_index_by_number(csr_number);
    *intr = 0;
    if (index == (uint8_t) -1) {
        *intr = 1;
        return;
    }
    uint32_t prev_value = csr_read(index);
    mem_register_write(rd, prev_value);
    if (uimm) {
        csr_write(index, prev_value & (~uimm));
    }
}

void zicnt_cycle_tick(void) {
    cycle++;

    control_status_registers[CSR_idx_cycle] = cycle & 0xffffffff;
    control_status_registers[CSR_idx_cycleh] = (cycle >> 32) & 0xffffffff;
}

void zicnt_time_tick(void) {
    static clock_t last_clk = 0;
    csr_time += (uint64_t) (ZICNT_TIMER_FREQ * ((double) (clock() - last_clk)) / CLOCKS_PER_SEC);

    timecmp = (((uint64_t) control_status_registers[CSR_idx_stimecmph]) << 32) |
              control_status_registers[CSR_idx_stimecmp];

    if (csr_time > timecmp && timecmp > 0) {
        trap_throw_interrupt(INTERRUPT_SUPERVISOR_TIMER);
    } else {
        trap_clear_interrupt_pending(INTERRUPT_SUPERVISOR_TIMER);
    }

    control_status_registers[CSR_idx_time] = csr_time & 0xffffffff;
    control_status_registers[CSR_idx_timeh] = (csr_time >> 32) & 0xffffffff;

    last_clk = clock();
}

uint64_t zicnt_get_cycle(void) {
    return cycle;
}
