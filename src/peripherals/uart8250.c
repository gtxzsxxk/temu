//
// Created by hanyuan on 2024/2/14.
//

#include "port/lock.h"
#include "port/console.h"
#include "uart8250.h"
#include "plic-simple.h"

#define FCR_INT_TRIGGER_LEVEL           (FCR >> 6)
#define LOOPBACK_MODE_ON                (MCR & (1 << 4))
#define INT_READ_DATA_AVAILABLE          0
#define INT_TRANSMIT_HOLDING_EMPTY       1
#define INT_RECEIVER_LINE_STATUS         2
#define INT_MODERN_STATUS                3

const uint8_t IIR_VALUE[] = {
        [INT_READ_DATA_AVAILABLE] = 0x2,
        [INT_TRANSMIT_HOLDING_EMPTY] = 0x1,
        [INT_RECEIVER_LINE_STATUS] = 0x3,
        [INT_MODERN_STATUS] = 0x0,
};

const uint8_t INT_PRIOR[] = {
        [INT_READ_DATA_AVAILABLE] = 0x2,
        [INT_TRANSMIT_HOLDING_EMPTY] = 0x4,
        [INT_RECEIVER_LINE_STATUS] = 0x1,
        [INT_MODERN_STATUS] = 0x5,
};

const uint8_t FCR_TRIGGER_LEVLEL[] = {
        [0x0] = 1,
        [0x1] = 4,
        [0x2] = 8,
        [0x3] = 14,
};

uint8_t current_pending_int = 0xff;

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

PORT_LOCK_T rx_fifo_lock;

static void throw_interrupt(uint8_t cause);

static void clear_interrupt(uint8_t cause);

static void write_rx_fifo(uint8_t data) {
    port_lock_lock(&rx_fifo_lock, 1);
    if (rx_fifo_tail < UART_FIFO_SIZE) {
        rx_fifo[rx_fifo_tail++] = data;
    }
    port_lock_unlock(&rx_fifo_lock);
}

static uint8_t read_from_rx_fifo(void) {
    port_lock_lock(&rx_fifo_lock, 1);
    uint8_t head = rx_fifo[0];
    if (rx_fifo_tail) {
        /* 移位 */
        for (int i = 0; i < UART_FIFO_SIZE - 1; i++) {
            rx_fifo[i] = rx_fifo[i + 1];
        }
        rx_fifo_tail--;
    }
    if (rx_fifo_tail) {
        LSR |= 1;
    } else {
        LSR &= ~1;
    }
    if (rx_fifo_tail < FCR_TRIGGER_LEVLEL[FCR_INT_TRIGGER_LEVEL]) {
        clear_interrupt(INT_READ_DATA_AVAILABLE);
    }
    port_lock_unlock(&rx_fifo_lock);
    return head;
}

static void clear_rx_fifo(void) {
    /* TODO: But it doesn’t clear the shift register, i.e. receiving of the current character continues. */
    port_lock_lock(&rx_fifo_lock, 1);
    rx_fifo_tail = 0;
    port_lock_unlock(&rx_fifo_lock);
}

static void clear_tx_fifo(void) {
    /* TODO: The shift register is not cleared, i.e. transmitting of the current character continues. */
    tx_fifo_tail = 0;
}

static void throw_interrupt(uint8_t cause) {
    if (!(IIR & 0x01)) {
        /* Is pending, check the priority */
        if (current_pending_int != 0xff) {
            if (INT_PRIOR[cause] > INT_PRIOR[current_pending_int]) {
                return;
            }
        }
    }
    if (!(IER & (1 << cause))) {
        /* Disabled interrupt */
        return;
    }
    IIR |= (IIR_VALUE[cause] << 1);
    IIR &= ~1;
    current_pending_int = cause;
    plic_throw_interrupt(PLIC_INT_LINE_UART);
}

static void clear_interrupt(uint8_t cause) {
    if (current_pending_int == cause) {
        IIR |= 0x01;
        current_pending_int = 0xff;
        plic_clear_pending(PLIC_INT_LINE_UART);
    }
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
            scratch = IIR;
            clear_interrupt(INT_TRANSMIT_HOLDING_EMPTY);
            return scratch;
        case 3:
            return LCR;
        case 4:
            return MCR;
        case 5:
            scratch = LSR;
            port_lock_lock(&rx_fifo_lock, 0);
            LSR &= ~(1 << 1);
            port_lock_unlock(&rx_fifo_lock);
            clear_interrupt(INT_RECEIVER_LINE_STATUS);
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
                    port_lock_lock(&rx_fifo_lock, 0);
                    LSR &= ~(1 << 5);
                    LSR &= ~(1 << 6);
                    clear_interrupt(INT_TRANSMIT_HOLDING_EMPTY);
                    port_lock_unlock(&rx_fifo_lock);
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
                port_console_write(tx_fifo[0]);
            }
            /* 移位 */
            for (int i = 0; i < UART_FIFO_SIZE - 1; i++) {
                tx_fifo[i] = tx_fifo[i + 1];
            }
            /* 指针移位 */
            tx_fifo_tail--;
            if (tx_fifo_tail == 0) {
                /* 1 << 6: Transmitter empty */
                port_lock_lock(&rx_fifo_lock, 0);
                LSR |= (1 << 6);
                port_console_flush();
                port_lock_unlock(&rx_fifo_lock);
            }
        }
        if (!tx_fifo_tail) {
            /* 1 << 5: Transmit FIFO empty */
            port_lock_lock(&rx_fifo_lock, 0);
            LSR |= (1 << 5);
            throw_interrupt(INT_TRANSMIT_HOLDING_EMPTY);
            port_lock_unlock(&rx_fifo_lock);
        }
    } else {
        div_cnt++;
    }
}

int uart8250_init(void) {
#ifndef BARE_METAL_PLATFORM
    pthread_t listening_thd;

    port_lock_init(&rx_fifo_lock);

    if (pthread_create(&listening_thd, NULL, uart8250_listening, NULL)) {
        return -1;
    }
#endif
    return 0;
}

#if defined(WIN32) || defined(WIN64)

#include <windows.h>

#endif

_Noreturn void *uart8250_listening(void *ptr) {
    for (;;) {
        int ch = port_console_read();
        port_lock_lock(&rx_fifo_lock, 1);
        if (rx_fifo_tail >= UART_FIFO_SIZE - 1) {
            /* FIFO is full */
            LSR |= 1 << 1;
            throw_interrupt(INT_RECEIVER_LINE_STATUS);
        } else {
            rx_fifo[rx_fifo_tail++] = ch;
        }
        if (rx_fifo_tail >= 1) {
            LSR |= 1;
        } else {
            LSR &= ~1;
        }
        throw_interrupt(INT_READ_DATA_AVAILABLE);
        port_lock_unlock(&rx_fifo_lock);
    }
}
