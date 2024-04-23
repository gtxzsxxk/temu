//
// Created by hanyuan on 2024/4/23.
//
#include "port/console.h"

#if !defined(WIN32) && !defined(WIN64)
#include <termios.h>
#include <unistd.h>
#else
#warning Console settings are not ported to windows currently
#endif

void port_os_console_init() {
#if !defined(WIN32) && !defined(WIN64)
    static struct termios tm;
    tcgetattr(STDIN_FILENO, &tm);
    cfmakeraw(&tm);
    tm.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &tm);
#else
#endif
}
