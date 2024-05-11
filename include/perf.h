//
// Created by hanyuan on 2024/4/28.
//

#ifndef TEMU_PERF_H
#define TEMU_PERF_H

#ifndef BARE_METAL_PLATFORM
#include <time.h>
#include <stdio.h>
#include "parameters.h"

#define PERF_BATCH_1M       1000000ul
#define PERF_BATCH_10M      10000000ul
#define PERF_BATCH_100M     100000000ul
#define PERF_BATCH_1000M    1000000000ul

#define PERF_MUTE 1

/* defined in CMakeLists.txt */
#if defined(PERF_MUTE_ALL)
#define PERF_MONITOR_CONTINUOUS(name, batch_size) do {} while(0);
#else
#define PERF_MONITOR_CONTINUOUS(name, batch_size) do { \
    static uint32_t counter##name = 0;            \
    static clock_t last_clk##name = 0;            \
    counter##name += 1;                           \
    if(unlikely(counter##name == batch_size)) {   \
        counter##name = 0;                        \
        double duration =                   \
            ((double)(clock()-last_clk##name))/CLOCKS_PER_SEC; \
        last_clk##name = clock();                 \
        printf(TEMU_PRINT_BANNER"%s takes %.5lf seconds.\n", \
#name, duration);    \
    }                                       \
    } while(0)
#endif


#if defined(PERF_MUTE) || defined(PERF_MUTE_ALL)
#define PERF_MONITOR_CONTINUOUS_CAN_MUTE(name, batch_size) do {} while(0);
#define PERF_MONITOR_DISCRETE_WARP_START(name) do {} while (0);
#define PERF_MONITOR_DISCRETE_WARP_END(name, batch_size) do {} while (0);
#else
#define PERF_MONITOR_CONTINUOUS_CAN_MUTE(name, batch_size) PERF_MONITOR_CONTINUOUS(name, batch_size)

#define PERF_MONITOR_DISCRETE_WARP_START(name) static uint32_t counter##name = 0; \
                                                static uint32_t clkstart##name = 0; \
                                                static double clksum##name = 0; \
                                                clkstart##name = clock();         \
                                                counter##name++;

#define PERF_MONITOR_DISCRETE_WARP_END(name, batch_size) do {\
                    clksum##name += ((double)(clock()-clkstart##name))/CLOCKS_PER_SEC;       \
                    if(unlikely(counter##name == batch_size)) {   \
                            counter##name = 0;                        \
                            printf(TEMU_PRINT_BANNER"%s takes %.5lf seconds.\n", \
#name, clksum##name);                                                        \
                            clksum##name = 0;                                                                                 \
                        }                                       \
                    } while(0)
#endif
#else
#define PERF_MONITOR_CONTINUOUS(name, batch_size) do {} while(0);
#define PERF_MONITOR_CONTINUOUS_CAN_MUTE(name, batch_size) do {} while(0);
#define PERF_MONITOR_DISCRETE_WARP_START(name) do {} while (0);
#define PERF_MONITOR_DISCRETE_WARP_END(name, batch_size) do {} while (0);
#endif

#endif //TEMU_PERF_H
