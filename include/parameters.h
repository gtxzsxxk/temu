//
// Created by hanyuan on 2024/2/8.
//

#ifndef TEMU_PARAMETERS_H
#define TEMU_PARAMETERS_H

//#define BARE_METAL_PLATFORM

#define RAM_BASE_ADDR               0x80000000
#define RAM_SIZE                    0x02000000

#define UART_BASE_ADDR              0x12500000
#define UART_SIZE                   0x00000100

#define PLIC_BASE_ADDR              0x0c000000
#define PLIC_SIZE                   0x00400000

/* 执行多少条指令后，让CSR timer tick */
#define ZICNT_TICK_INTERVAL         10000
/* CSR timer的频率。此项需要与设备树一致 */
#define ZICNT_TIMER_FREQ            100000000

#define TEMU_PRINT_BANNER           "[TEMU MESSAGE] "

#define TEMU_DEBUG_CODE             0

#define likely(x)                   __builtin_expect(!!(x), 1)
#define unlikely(x)                 __builtin_expect(!!(x), 0)

#endif //TEMU_PARAMETERS_H
