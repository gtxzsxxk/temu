//
// Created by hanyuan on 2024/2/10.
//

#ifndef TEMU_ZICSR_H
#define TEMU_ZICSR_H

#define CSR_SIZE 40

#define CSR_MASK_USER 0
#define CSR_MASK_SUPERVISOR 1
#define CSR_MASK_MACHINE 3

#define CSR_MASK_READ 1 << 3
#define CSR_MASK_WRITE 1 << 4

#define CSR_RESET_VALUE(name, value) [CSR_idx_##name] = value,
#define CSR_MATCH_DECLARE(name) [CSR_idx_##name] = CSR_prv_##name | (CSR_num_##name << 8),

/* === csrIndexGen match begin DO NOT MODIFY THIS LINE === */
#define CSR_num_sstatus 0x100
#define CSR_prv_sstatus (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_sstatus 0

#define CSR_num_sie 0x104
#define CSR_prv_sie (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_sie 1

#define CSR_num_stvec 0x105
#define CSR_prv_stvec (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_stvec 2

#define CSR_num_scounteren 0x106
#define CSR_prv_scounteren (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_scounteren 3

#define CSR_num_senvcfg 0x10A
#define CSR_prv_senvcfg (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_senvcfg 4

#define CSR_num_sscratch 0x140
#define CSR_prv_sscratch (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_sscratch 5

#define CSR_num_sepc 0x141
#define CSR_prv_sepc (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_sepc 6

#define CSR_num_scause 0x142
#define CSR_prv_scause (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_scause 7

#define CSR_num_stval 0x143
#define CSR_prv_stval (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_stval 8

#define CSR_num_sip 0x144
#define CSR_prv_sip (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_sip 9

#define CSR_num_stimecmp 0x14D
#define CSR_prv_stimecmp (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_stimecmp 10

#define CSR_num_stimecmph 0x15D
#define CSR_prv_stimecmph (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_stimecmph 11

#define CSR_num_satp 0x180
#define CSR_prv_satp (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_satp 12

#define CSR_num_mstatus 0x300
#define CSR_prv_mstatus (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mstatus 13

#define CSR_num_misa 0x301
#define CSR_prv_misa (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_misa 14

#define CSR_num_medeleg 0x302
#define CSR_prv_medeleg (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_medeleg 15

#define CSR_num_mideleg 0x303
#define CSR_prv_mideleg (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mideleg 16

#define CSR_num_mie 0x304
#define CSR_prv_mie (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mie 17

#define CSR_num_mtvec 0x305
#define CSR_prv_mtvec (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mtvec 18

#define CSR_num_mcounteren 0x306
#define CSR_prv_mcounteren (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mcounteren 19

#define CSR_num_menvcfg 0x30A
#define CSR_prv_menvcfg (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_menvcfg 20

#define CSR_num_mstatush 0x310
#define CSR_prv_mstatush (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mstatush 21

#define CSR_num_menvcfgh 0x31A
#define CSR_prv_menvcfgh (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_menvcfgh 22

#define CSR_num_mcountinhibit 0x320
#define CSR_prv_mcountinhibit (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mcountinhibit 23

#define CSR_num_mscratch 0x340
#define CSR_prv_mscratch (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mscratch 24

#define CSR_num_mepc 0x341
#define CSR_prv_mepc (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mepc 25

#define CSR_num_mcause 0x342
#define CSR_prv_mcause (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mcause 26

#define CSR_num_mtval 0x343
#define CSR_prv_mtval (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mtval 27

#define CSR_num_mip 0x344
#define CSR_prv_mip (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mip 28

#define CSR_num_mtinst 0x34A
#define CSR_prv_mtinst (CSR_MASK_MACHINE | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_mtinst 29

#define CSR_num_scontext 0x5A8
#define CSR_prv_scontext (CSR_MASK_SUPERVISOR | CSR_MASK_READ | CSR_MASK_WRITE)
#define CSR_idx_scontext 30

#define CSR_num_cycle 0xC00
#define CSR_prv_cycle (CSR_MASK_USER | CSR_MASK_READ)
#define CSR_idx_cycle 31

#define CSR_num_time 0xC01
#define CSR_prv_time (CSR_MASK_USER | CSR_MASK_READ)
#define CSR_idx_time 32

#define CSR_num_cycleh 0xC80
#define CSR_prv_cycleh (CSR_MASK_USER | CSR_MASK_READ)
#define CSR_idx_cycleh 33

#define CSR_num_timeh 0xC81
#define CSR_prv_timeh (CSR_MASK_USER | CSR_MASK_READ)
#define CSR_idx_timeh 34

#define CSR_num_mvendorid 0xF11
#define CSR_prv_mvendorid (CSR_MASK_MACHINE | CSR_MASK_READ)
#define CSR_idx_mvendorid 35

#define CSR_num_marchid 0xF12
#define CSR_prv_marchid (CSR_MASK_MACHINE | CSR_MASK_READ)
#define CSR_idx_marchid 36

#define CSR_num_mimpid 0xF13
#define CSR_prv_mimpid (CSR_MASK_MACHINE | CSR_MASK_READ)
#define CSR_idx_mimpid 37

#define CSR_num_mhartid 0xF14
#define CSR_prv_mhartid (CSR_MASK_MACHINE | CSR_MASK_READ)
#define CSR_idx_mhartid 38

#define CSR_num_mconfigptr 0xF15
#define CSR_prv_mconfigptr (CSR_MASK_MACHINE | CSR_MASK_READ)
#define CSR_idx_mconfigptr 39

/* === csrIndexGen match end DO NOT MODIFY THIS LINE === */

#define mstatus_MIE 3
#define mstatus_SIE 1
#define mstatus_MPIE 7
#define mstatus_SPIE 5
#define mstatus_MPP 11
#define mstatus_SPP 8
#define mstatus_MPRV 17
#define mstatus_SUM  18
#define mstatus_MXR  19

#define sstatus_SIE 1
#define sstatus_SPIE 5
#define sstatus_SPP 8

extern uint8_t current_privilege;
extern uint32_t control_status_registers[];

void csr_csrrw(uint8_t rs1, uint8_t rd, uint16_t csr_number, uint8_t *intr);

void csr_csrrs(uint8_t rs1, uint8_t rd, uint16_t csr_number, uint8_t *intr);

void csr_csrrc(uint8_t rs1, uint8_t rd, uint16_t csr_number, uint8_t *intr);

void csr_csrrwi(uint8_t uimm, uint8_t rd, uint16_t csr_number, uint8_t *intr);

void csr_csrrsi(uint8_t uimm, uint8_t rd, uint16_t csr_number, uint8_t *intr);

void csr_csrrci(uint8_t uimm, uint8_t rd, uint16_t csr_number, uint8_t *intr);

void zicnt_cycle_tick(void);

void zicnt_time_tick(void);

uint64_t zicnt_get_cycle(void);

void zicnt_init(void);

#endif //TEMU_ZICSR_H
