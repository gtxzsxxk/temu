//
// Created by hanyuan on 2024/2/10.
//
#include <stdint.h>
#include "mem.h"
#include "zicsr.h"

uint32_t control_status_registers[CSR_SIZE] = {
        CSR_RESET_VALUE(misa, 0x40140101)
        CSR_RESET_VALUE(mvendorid, 0x00000000)
        CSR_RESET_VALUE(mhartid, 0x00000000)

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
        CSR_MATCH_DECLARE(satp)
        CSR_MATCH_DECLARE(mstatus)
        CSR_MATCH_DECLARE(misa)
        CSR_MATCH_DECLARE(medeleg)
        CSR_MATCH_DECLARE(mideleg)
        CSR_MATCH_DECLARE(mie)
        CSR_MATCH_DECLARE(mtvec)
        CSR_MATCH_DECLARE(mcounteren)
        CSR_MATCH_DECLARE(mstatush)
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
        /* Raise exception */
        return -1;
    }
    for (; start_index < CSR_SIZE; start_index++) {
        if (((csr_match[start_index] >> 8) & 0xFFF) == csr_number) {
            return start_index;
        }
    }
    /* Raise exception */
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

void csr_csrrw(uint8_t rs1, uint8_t rd, uint16_t csr_number) {
    uint8_t index = csr_get_index_by_number(csr_number);
    if (index == (uint8_t) -1) {
        /* Raise exception */
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

void csr_csrrs(uint8_t rs1, uint8_t rd, uint16_t csr_number) {
    uint8_t index = csr_get_index_by_number(csr_number);
    if (index == (uint8_t) -1) {
        /* Raise exception */
        return;
    }
    uint32_t prev_value = csr_read(index);
    mem_register_write(rd, prev_value);
    if (rs1) {
        csr_write(index, prev_value | mem_register_read(rs1));
    }
}

void csr_csrrc(uint8_t rs1, uint8_t rd, uint16_t csr_number) {
    uint8_t index = csr_get_index_by_number(csr_number);
    if (index == (uint8_t) -1) {
        /* Raise exception */
        return;
    }
    uint32_t prev_value = csr_read(index);
    mem_register_write(rd, prev_value);
    if (rs1) {
        csr_write(index, prev_value & (~mem_register_read(rs1)));
    }
}
