//
// Created by hanyuan on 2024/2/8.
//

#ifndef TEMU_PARAMETERS_H
#define TEMU_PARAMETERS_H

#define ROM_BASE_ADDR           0x00000000
#define ROM_SIZE                0x00010000
#define RAM_BASE_ADDR           0x02000000
#define RAM_SIZE                0x01000000

#define UART_BASE_ADDR          0x12500000
#define UART_SIZE               0x00000100

/* 均摊时钟 in MHz */
#define SIM_EVENLY_DIV_CLOCK    100
/* 缩放比例 */
#define SIM_CLOCK_SCALE    10
/* 执行多少条指令后，统一给出CPU */
#define SIM_YIELD_GAP           10000
/* 给出CPU的时间(us) */
#define SIM_YIELD_TIME          (SIM_YIELD_GAP/SIM_EVENLY_DIV_CLOCK*SIM_CLOCK_SCALE)
/* 此时均摊到每一条指令，平均时钟为100MHz */
#define ZICNT_TIMER_FREQ        (SIM_EVENLY_DIV_CLOCK*1000000/SIM_YIELD_GAP)

#endif //TEMU_PARAMETERS_H
