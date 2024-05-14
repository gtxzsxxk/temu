//
// Created by hanyuan on 2024/5/10.
//

#ifndef TEMU_SYSTEM_TIMER_H
#define TEMU_SYSTEM_TIMER_H

#ifndef BARE_METAL_PLATFORM
#include <time.h>
#define port_clock_t clock_t
#define PORT_CLOCKS_PER_SEC CLOCKS_PER_SEC
#else
#define port_clock_t uint64_t
#define PORT_CLOCKS_PER_SEC 1000000
#endif

port_clock_t port_system_timer_get_ticks(void);

#endif //TEMU_SYSTEM_TIMER_H
