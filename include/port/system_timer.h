//
// Created by hanyuan on 2024/5/10.
//

#ifndef TEMU_SYSTEM_TIMER_H
#define TEMU_SYSTEM_TIMER_H

#define port_clock_t long int
#define PORT_CLOCKS_PER_SEC 1000000

port_clock_t port_system_timer_get_ticks(void);

#endif //TEMU_SYSTEM_TIMER_H
