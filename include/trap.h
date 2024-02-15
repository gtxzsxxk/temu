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

void trap_throw_interrupt(uint32_t cause);

void trap_throw_exception(uint32_t cause);

#endif //TEMU_TRAP_H
