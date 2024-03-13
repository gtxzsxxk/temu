//
// Created by hanyuan on 2024/2/14.
//
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include "uart8250.h"

uint8_t IER = 0x00;
uint8_t IIR = 0xc1;
uint8_t FCR = 0xc0;
uint8_t LCR = 0x03;
uint8_t MCR = 0;
uint8_t LSR = 0;
uint8_t MSR = 0;

uint16_t divisor;
uint16_t div_cnt = 0;

uint8_t tx_fifo[UART_FIFO_SIZE];
uint8_t tx_fifo_tail = 0;

uint8_t rx_fifo[UART_FIFO_SIZE];
uint8_t rx_fifo_tail = 0;

pthread_spinlock_t rx_fifo_lock;

static uint8_t read_from_rx_fifo(void) {
    pthread_spin_lock(&rx_fifo_lock);
    uint8_t head = rx_fifo[0];
    if (rx_fifo_tail) {
        /* 移位 */
        for (int i = 0; i < UART_FIFO_SIZE - 1; i++) {
            rx_fifo[i] = rx_fifo[i + 1];
        }
        rx_fifo_tail--;
    }
    pthread_spin_unlock(&rx_fifo_lock);
    return head;
}

static void clear_rx_fifo(void) {
    /* TODO: But it doesn’t clear the shift register, i.e. receiving of the current character continues. */
    pthread_spin_lock(&rx_fifo_lock);
    rx_fifo_tail = 0;
    pthread_spin_unlock(&rx_fifo_lock);
}

static void clear_tx_fifo(void) {
    /* TODO: The shift register is not cleared, i.e. transmitting of the current character continues. */
    tx_fifo_tail = 0;
}

uint8_t uart8250_read_b(uint8_t offset) {
    uint8_t scratch;
    switch (offset) {
        case 0:
            if (LCR >> 7) {
                return divisor & 0x00ff;
            }
            return read_from_rx_fifo();
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
            scratch = LSR;
            LSR &= ~(1 << 1);
            return scratch;
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
                if (tx_fifo_tail < UART_FIFO_SIZE) {
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
                IER = data & 0x0f;
            }
            break;
        case 2:
            FCR = data;
            if (FCR & (1 << 1)) {
                clear_rx_fifo();
            }
            if (FCR & (1 << 2)) {
                clear_tx_fifo();
            }
            break;
        case 3:
            LCR = data;
            break;
        case 4:
            MCR = data;
            if (LOOPBACK_MODE_ON) {
                /* Out2 */
                if (MCR & (1 << 3)) {
                    MSR |= 0x80;
                } else {
                    MSR &= ~0x80;
                }
                /* Out1 */
                if (MCR & (1 << 2)) {
                    MSR |= 0x40;
                } else {
                    MSR &= ~0x40;
                }
                /* RTS */
                if (MCR & (1 << 1)) {
                    MSR |= 0x10;
                } else {
                    MSR &= ~0x10;
                }
                /* DTR */
                if (MCR & (1 << 0)) {
                    MSR |= 0x20;
                } else {
                    MSR &= ~0x20;
                }
            } else {
                MSR &= ~0xc0;
            }
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
            if (LOOPBACK_MODE_ON) {
                write_rx_fifo(tx_fifo[0]);
            } else {
                printf("%c", tx_fifo[0]);
            }
            /* 移位 */
            for (int i = 0; i < UART_FIFO_SIZE - 1; i++) {
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

int uart8250_init(void) {
    pthread_t listening_thd;

    pthread_spin_init(&rx_fifo_lock, PTHREAD_PROCESS_PRIVATE);

    if (pthread_create(&listening_thd, NULL, uart8250_listening, NULL)) {
        printf("Failed to create uart listening thread\n");
        return -1;
    }

    return 0;
}

_Noreturn void *uart8250_listening(void *ptr) {
    for (;;) {
        int ch = getchar();
        pthread_spin_lock(&rx_fifo_lock);
        if (rx_fifo_tail >= UART_FIFO_SIZE - 1) {
            /* FIFO is full */
            LSR |= 1 << 1;
        } else {
            rx_fifo[rx_fifo_tail++] = ch;
        }
        if (rx_fifo_tail >= 1) {
            LSR |= 1;
        } else {
            LSR &= ~1;
        }
        pthread_spin_unlock(&rx_fifo_lock);
    }
}
