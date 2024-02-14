//
// Created by hanyuan on 2024/2/14.
//

#ifndef TEMU_UART8250_H
#define TEMU_UART8250_H

uint8_t uart8250_read_b(uint8_t offset);

void uart8250_write_b(uint8_t offset, uint8_t data);

void uart8250_tick(void);

#endif //TEMU_UART8250_H
