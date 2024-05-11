//
// Created by hanyuan on 2024/5/11.
//

#include <unistd.h>
#include "port/system_timer.h"
#include "port/os_yield_cpu.h"

void port_os_yield_cpu(void) {
#ifndef BARE_METAL_PLATFORM
    static port_clock_t last_yield_tick = 0;
    port_clock_t current_tick = port_system_timer_get_ticks();
    if (current_tick - last_yield_tick > PORT_OS_YIELD_CPU_EVERY_MS * (PORT_CLOCKS_PER_SEC / 1000)) {
        usleep(10000);
        last_yield_tick = current_tick;
    }
#endif
}
