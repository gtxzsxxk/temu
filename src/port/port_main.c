//
// Created by hanyuan on 2024/5/11.
//

#include <stdint.h>
#include <string.h>
#include "port/load_binary.h"
#include "machine.h"
#include "port/port_main.h"

/**
  * Usage: temu -exec=program.bin [-with=addr#file.bin]
  *
  * Example:
  * --addr=0x81fa0000
  * --exec=fw_jump.bin
  * --with=0x80000000#u-boot.bin
  * --with=0x81ffd800#u-boot.dtb
  * --with=0x81000000#uImage.gz
 */

const char temu_default_args[] = "--addr=0x81fa0000 --exec=fw_jump.bin --with=0x80000000#u-boot.bin --with=0x81ffd800#u-boot.dtb --with=0x81000000#uImage.gz";

int port_main(int argc, char **argv) {
    /* TODO: optimize or refactor this argument parsing */
    char buffer[32] = {0};
    char exec_path[64] = {0};
    uint32_t start_addr = 0;
    int has_exec = 0, has_addr = 0;
    for (int i = 1; i < argc; i++) {
        for (int j = 0; j < 32; j++) {
            if (argv[i][j + 2] == '=') {
                buffer[j] = '\0';
                break;
            }
            buffer[j] = argv[i][j + 2];
        }
        if (!strcmp(buffer, "addr")) {
            int counter = 7;
            for (int j = 9; j < strlen(argv[i]); j++) {
                char literal_value = argv[i][j];
                if (literal_value >= 'a') {
                    literal_value -= 'a';
                    literal_value += 10;
                } else if (literal_value >= 'A') {
                    literal_value -= 'A';
                    literal_value += 10;
                } else if (literal_value >= '0') {
                    literal_value -= '0';
                }
                start_addr |= (literal_value << (counter << 2));
                counter--;
            }
            has_addr = 1;
        } else if (!strcmp(buffer, "exec")) {
            int j = 7;
            for (; j < strlen(argv[i]); j++) {
                exec_path[j - 7] = argv[i][j];
            }
            exec_path[j - 7] = '\0';
            has_exec = 1;
        } else if (!strcmp(buffer, "with")) {
            int counter = 7, j = 9;
            uint32_t addr = 0;
            for (; j < 9 + 8; j++) {
                char literal_value = argv[i][j];
                if (literal_value >= 'a') {
                    literal_value -= 'a';
                    literal_value += 10;
                } else if (literal_value >= 'A') {
                    literal_value -= 'A';
                    literal_value += 10;
                } else if (literal_value >= '0') {
                    literal_value -= '0';
                }
                addr |= (literal_value << (counter << 2));
                counter--;
            }
            for (j = 18; j < strlen(argv[i]); j++) {
                buffer[j - 18] = argv[i][j];
            }
            buffer[j - 18] = '\0';
            port_load_binary_from_file(buffer, addr);
        }
    }

    if (!has_exec || !has_addr) {
        return -1;
    }

    port_load_binary_from_file(exec_path, start_addr);
    machine_start(start_addr, 0);
    return 0;
}
