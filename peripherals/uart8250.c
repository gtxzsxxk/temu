//
// Created by hanyuan on 2024/2/14.
//
#include <stdint.h>
#include <stdio.h>
#include "uart8250.h"

uint8_t RCB = 0;
uint8_t IER = 0x00;
uint8_t IIR = 0xc1;
uint8_t FCR = 0xc0;
uint8_t LCR = 0x03;
uint8_t MCR = 0;
uint8_t LSR = 0;
uint8_t MSR = 0;

uint16_t divisor;
uint16_t div_cnt = 0;

uint8_t tx_fifo[32];
uint8_t tx_fifo_tail = 0;

uint8_t uart8250_read_b(uint8_t offset) {
    switch (offset) {
        case 0:
            if (LCR >> 7) {
                return divisor & 0x00ff;
            }
            return RCB;
        case 1:
            if (LCR >> 7) {
                return (divisor >> 8) & 0x00ff;
            }
            return IER;
        case 2:
            return IIR;
        case 3:
            return LCR;
        case 5:
            return LSR;
        case 6:
            return MSR;
        default:
            /* exception */
            return 0;
    }
}

void uart8250_write_b(uint8_t offset, uint8_t data) {
    switch (offset) {
        case 0:
            if (LCR >> 7) {
                divisor &= 0xff00;
                divisor |= data;
            } else {
                if (tx_fifo_tail < 32) {
                    tx_fifo[tx_fifo_tail++] = data;
                    /* 1 << 5: Transmit FIFO empty */
                    LSR &= ~(1 << 5);
                    LSR &= ~(1 << 6);
                }
            }
            break;
        case 1:
            if (LCR >> 7) {
                divisor &= 0x00ff;
                divisor |= (data << 8);
            } else {
                IER = data;
            }
            break;
        case 2:
            FCR = data;
            break;
        case 3:
            LCR = data;
            break;
        case 4:
            MCR = data;
            break;
        default:
            /* exception */
            break;
    }
}

void uart8250_tick(void) {
    if (div_cnt >= divisor) {
        div_cnt = 0;
        if (tx_fifo_tail) {
            /* 发送当前的队首 */
            printf("%c", tx_fifo[0]);
            /* 移位 */
            for (int i = 0; i < 32 - 1; i++) {
                tx_fifo[i] = tx_fifo[i + 1];
            }
            /* 指针移位 */
            tx_fifo_tail--;
            if (tx_fifo_tail == 0) {
                /* 1 << 6: Transmitter empty */
                LSR |= (1 << 6);
            }
        }
        if (!tx_fifo_tail) {
            /* 1 << 5: Transmit FIFO empty */
            LSR |= (1 << 5);
        }
    } else {
        div_cnt++;
    }
}