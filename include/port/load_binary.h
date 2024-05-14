//
// Created by hanyuan on 2024/4/23.
//

#ifndef TEMU_LOAD_BINARY_H
#define TEMU_LOAD_BINARY_H

#include <stdint.h>

#ifndef BARE_METAL_PLATFORM

#include <stdio.h>

#define PORT_FILE_OPEN fopen
#define PORT_FILE_EOF feof
#define PORT_FILE_READ fread
#define PORT_FILE_CLOSE fclose
#else
#error Implement the file operations for BARE METAL PLATFORMs
#endif

int port_load_binary_from_file(const char *path, uint32_t addr);

#endif //TEMU_LOAD_BINARY_H
