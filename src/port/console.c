//
// Created by hanyuan on 2024/4/23.
//
#include "port/console.h"

#if !defined(WIN32) && !defined(WIN64) && !defined(STM32)

#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#else

#include <windows.h>

#endif

void port_os_console_init() {
#if !defined(WIN32) && !defined(WIN64)
    static struct termios tm;
    tcgetattr(STDIN_FILENO, &tm);
    cfmakeraw(&tm);
    tm.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &tm);
#else
    HANDLE hStdin;
    DWORD fdwSaveOldMode, fdwMode;
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &fdwSaveOldMode);
    fdwMode = ENABLE_PROCESSED_INPUT | ENABLE_INSERT_MODE | ENABLE_VIRTUAL_TERMINAL_INPUT |
              ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hStdin, fdwMode);
#endif
}

void port_console_write(uint8_t c) {
#ifndef STM32
    printf("%c", c);
#else
#error port console write to stm32 platform!
#endif
}

void port_console_flush(void) {
#ifndef STM32
    fflush(stdout);
#endif
}

int port_console_read(void) {
#ifndef STM32
#if !defined(WIN32) && !defined(WIN64)
    return getchar();
#else
#error port console read to windows platform!
#endif
#else
#error port console write to stm32 platform!
#endif
}
