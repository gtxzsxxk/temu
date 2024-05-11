//
// Created by hanyuan on 2024/2/11.
//

#ifndef TEMU_TRAP_H
#define TEMU_TRAP_H

#define EXCEPTION_INST_ADDR_MISALIGNED              0
#define EXCEPTION_INST_ACCESS_FAULT                 1
#define EXCEPTION_ILLEGAL_INST                      2
#define EXCEPTION_LOAD_ADDR_MISALIGNED              4
#define EXCEPTION_LOAD_ACCESS_FAULT                 5
#define EXCEPTION_STORE_ADDR_MISALIGNED             6
#define EXCEPTION_STORE_ACCESS_FAULT                7
#define EXCEPTION_ECALL_FROM_U                      8
#define EXCEPTION_ECALL_FROM_S                      9
#define EXCEPTION_ECALL_FROM_M                      11
#define EXCEPTION_INST_PAGEFAULT                    12
#define EXCEPTION_LOAD_PAGEFAULT                    13
#define EXCEPTION_STORE_PAGEFAULT                   15

#define INTERRUPT_SUPERVISOR_SOFTWARE               1
#define INTERRUPT_MACHINE_SOFTWARE                  3
#define INTERRUPT_SUPERVISOR_TIMER                  5
#define INTERRUPT_MACHINE_TIMER                     7
#define INTERRUPT_SUPERVISOR_EXTERNAL               9
#define INTERRUPT_MACHINE_EXTERNAL                  11

extern uint8_t in_wfi;
extern uint32_t wfi_next_pc;

uint32_t trap_is_pending(void);

void trap_clear_interrupt_pending(uint32_t cause);

void trap_throw_interrupt(uint32_t cause);

void trap_take_interrupt(void);

void trap_throw_exception(uint32_t cause, uint32_t tval);

void trap_return_machine(void);

void trap_return_supervisor(void);

#endif //TEMU_TRAP_H
