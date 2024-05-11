//
// Created by hanyuan on 2024/5/11.
//

#ifndef TEMU_OS_YIELD_CPU_H
#define TEMU_OS_YIELD_CPU_H

#define PORT_OS_YIELD_CPU_EVERY_MS  50
#define PORT_OS_YIELD_CPU_INTC_LEN  16

void
port_os_yield_cpu_init(uint64_t (*time_conv)(uint64_t), uint64_t *zicnt_timer_ticks, uint64_t *zicnt_timer_compare);

void port_os_yield_cpu_add_interrupt(void (*intc)(void));

void port_os_yield_cpu(void);

#endif //TEMU_OS_YIELD_CPU_H
