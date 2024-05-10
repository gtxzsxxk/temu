//
// Created by hanyuan on 2024/5/10.
//

#include "port/system_timer.h"

#ifndef STM32

#include <time.h>

port_clock_t port_system_timer_get_ticks(void) {
    return clock();
}

#endif