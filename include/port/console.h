//
// Created by hanyuan on 2024/4/23.
//

#ifndef TEMU_CONSOLE_H
#define TEMU_CONSOLE_H
#include <stdint.h>
#include "parameters.h"

void port_os_console_init();

void port_console_write(uint8_t c);

void port_console_flush(void);

int port_console_read(void);

#endif //TEMU_CONSOLE_H
