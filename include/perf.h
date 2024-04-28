//
// Created by hanyuan on 2024/4/28.
//

#ifndef TEMU_PERF_H
#define TEMU_PERF_H

#include <time.h>
#include <stdio.h>
#include "parameters.h"

#define PERF_BATCH_10M      10000000ul
#define PERF_BATCH_100M     100000000ul
#define PERF_BATCH_1000M    1000000000ul

#define PERF_MUTE 0
#define PERF_MUTE_ALL 1

#if PERF_MUTE_ALL
#define PERF_MONITOR(name, batch_size) do { } while(0);
#else
#define PERF_MONITOR(name, batch_size) do { \
    static uint32_t counter = 0;            \
    static clock_t last_clk = 0;            \
    counter += 1;                           \
    if(unlikely(counter == batch_size)) {   \
        counter = 0;                        \
        double duration =                   \
            ((double)(clock()-last_clk))/CLOCKS_PER_SEC; \
        last_clk = clock();                 \
        printf(TEMU_PRINT_BANNER"%s takes %.5lf seconds.\n", \
            name, duration);    \
    }                                       \
    } while(0);
#endif

#if PERF_MUTE
#define PERF_MONITOR_CAN_MUTE(name, batch_size) do { } while(0);
#else
#define PERF_MONITOR_CAN_MUTE(name, batch_size) PERF_MONITOR(name, batch_size)
#endif

#endif //TEMU_PERF_H
