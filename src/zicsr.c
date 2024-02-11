//
// Created by hanyuan on 2024/2/10.
//
#include <stdint.h>
#include "zicsr.h"

uint32_t control_status_registers[CSR_SIZE] = {
        CSR_RESET_VALUE(misa,0x40140101)
        CSR_RESET_VALUE(mvendorid,0x00000000)
        CSR_RESET_VALUE(mhartid,0x00000000)

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
        [0x1] = CSR_idx_sstatus,
        [0x3] = CSR_idx_mstatus,
        [0x5] = CSR_idx_scontext,
        [0xC] = CSR_idx_cycle,
        [0xF] = CSR_idx_mvendorid,
};

uint8_t csr_get_index_by_number(uint16_t csr_number) {

}
