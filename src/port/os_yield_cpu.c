//
// Created by hanyuan on 2024/5/11.
//

#include <stdint.h>
#include "parameters.h"

#ifndef BARE_METAL_PLATFORM

#include <unistd.h>
#include "trap.h"
#include "port/system_timer.h"
#include "port/os_yield_cpu.h"

#ifndef NULL
#define NULL (void*)0
#endif

static uint64_t *emulation_timer_tick = NULL;

static uint64_t *emulation_timer_compare = NULL;

static uint64_t (*emulation_timer_conv)(uint64_t) = NULL;

static void (*interrupt_checks[PORT_OS_YIELD_CPU_INTC_LEN])(void) = {0};

static uint8_t intc_counter = 0;

void
port_os_yield_cpu_init(uint64_t (*time_conv)(uint64_t), uint64_t *zicnt_timer_ticks, uint64_t *zicnt_timer_compare) {
    emulation_timer_conv = time_conv;
    emulation_timer_tick = zicnt_timer_ticks;
    emulation_timer_compare = zicnt_timer_compare;
}

void port_os_yield_cpu_add_interrupt(void (*intc)(void)) {
    interrupt_checks[intc_counter++] = intc;
}

void port_os_yield_cpu(void) {
#ifndef BARE_METAL_PLATFORM
    static port_clock_t last_yield_tick = 0;
    port_clock_t before_sleep, after_sleep, sleep_counter = 0, current_tick = port_system_timer_get_ticks();

    if (current_tick - last_yield_tick > PORT_OS_YIELD_CPU_EVERY_MS * (PORT_CLOCKS_PER_SEC / 1000)) {
        int64_t ticks_should_elapse = (int64_t) (*emulation_timer_compare - *emulation_timer_tick);

        if (ticks_should_elapse > 0) {
            uint64_t sleep_us = emulation_timer_conv(ticks_should_elapse);

            if (sleep_us > 50) {
                sleep_us /= 35;

                while (sleep_counter < sleep_us) {
                    before_sleep = port_system_timer_get_ticks() * (1000000 / PORT_CLOCKS_PER_SEC);
                    usleep(50);

                    /* After the cpu is resumed we should check if we have any interrupt immediately */
                    for (uint8_t i = 0; i < intc_counter; i++) {
                        interrupt_checks[i]();
                    }
                    if (trap_is_pending()) {
                        return;
                    }

                    after_sleep = port_system_timer_get_ticks() * (1000000 / PORT_CLOCKS_PER_SEC);
                    sleep_counter += after_sleep - before_sleep;
                }
            }
        }

        last_yield_tick = current_tick;
    }
#endif
}

#else

void
port_os_yield_cpu_init(uint64_t (*time_conv)(uint64_t), uint64_t *zicnt_timer_ticks, uint64_t *zicnt_timer_compare) {}

void port_os_yield_cpu_add_interrupt(void (*intc)(void)) {}

void port_os_yield_cpu(void) {}

#endif
