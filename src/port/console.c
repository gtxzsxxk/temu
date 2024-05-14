//
// Created by hanyuan on 2024/4/23.
//
#include "port/console.h"
#include "uart8250.h"

#if !defined(WIN32) && !defined(WIN64) && !defined(BARE_METAL_PLATFORM)

#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#elif (defined(WIN32) || defined(WIN64)) && !defined(BARE_METAL_PLATFORM)

#include <windows.h>

#endif

void port_os_console_init() {
#if !defined(WIN32) && !defined(WIN64) && !defined(BARE_METAL_PLATFORM)
    static struct termios tm;
    tcgetattr(STDIN_FILENO, &tm);
    cfmakeraw(&tm);
    tm.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &tm);
#elif (defined(WIN32) || defined(WIN64)) && !defined(BARE_METAL_PLATFORM)
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

#ifndef BARE_METAL_PLATFORM

void port_console_write(uint8_t c) {
    printf("%c", c);
}

void port_console_flush(void) {
    fflush(stdout);
}

int port_console_read(void) {
#if !defined(WIN32) && !defined(WIN64)
    return getchar();
#else
#error port console read to windows platform!
#endif
}

#else
#warning Define function port_console_write outside of TEMU!
#warning Define function port_console_read outside of TEMU!
#endif
