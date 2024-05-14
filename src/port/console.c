//
// Created by hanyuan on 2024/4/23.
//
#include "port/console.h"

#if !defined(WIN32) && !defined(WIN64) && !defined(BARE_METAL_PLATFORM)

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

    uart8250_init();
}

void port_console_write(uint8_t c) {
#ifndef BARE_METAL_PLATFORM
    printf("%c", c);
#else
#error port console write to bare metal platform!
#endif
}

void port_console_flush(void) {
#ifndef BARE_METAL_PLATFORM
    fflush(stdout);
#endif
}

int port_console_read(void) {
#ifndef BARE_METAL_PLATFORM
#if !defined(WIN32) && !defined(WIN64)
    return getchar();
#else
#error port console read to windows platform!
#endif
#else
#error port console write to bare metal platform!
#endif
}
