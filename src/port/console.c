//
// Created by hanyuan on 2024/4/23.
//
#include "port/console.h"

#if !defined(WIN32) && !defined(WIN64)
#include <termios.h>
#include <unistd.h>
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
