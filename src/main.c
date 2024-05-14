//
// Created by hanyuan on 2024/2/8.
//

#include "parameters.h"

#ifndef BARE_METAL_PLATFORM
#include "port/port_main.h"

int main(int argc, char **argv) {
    return port_main(argc, argv);
}

#endif
