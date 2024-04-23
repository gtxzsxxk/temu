//
// Created by hanyuan on 2024/2/8.
//
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include "machine.h"
#include "port/load_binary.h"

static const char *usage = "Usage: temu [-ram/-rom/-addr 0x02000000] [-printreg] -exec=program.bin [-with=addr#file.bin]\n\n"
                           "Example:\n"
                           "--addr=0x80db5000\n"
                           "--exec=fw_jump.bin\n"
                           "--with=0x80000000#u-boot.bin\n"
                           "--with=0x80dfd800#u-boot.dtb\n"
                           "--with=0x80ab5000#uImage.gz\n";

static int with_binary(const char *data);

int main(int argc, char **argv) {
    uint32_t start_addr = 0;
    int opt_index, exec_flag = 0, printreg = 0;
    char exec_path[64] = {0};

    struct option opts[] = {
            {"rom",      no_argument,       0, 0},
            {"ram",      no_argument,       0, 0},
            {"addr",     required_argument, 0, 0},
            {"printreg", no_argument,       0, 0},
            {"exec",     required_argument, 0, 0},
            {"with",     required_argument, 0, 0},
    };

    while (getopt_long(argc, argv, "", opts, &opt_index) != -1) {
        switch (opt_index) {
            case 0:
                start_addr = 0x00000000;
                break;
            case 1:
                start_addr = 0x02000000;
                break;
            case 2:
                if (optarg) {
                    sscanf(optarg, "%x", &start_addr);
                    exec_flag = 1;
                } else {
                    printf("%s", usage);
                    return -1;
                }
                break;
            case 3:
                printreg = 1;
                break;
            case 4:
                if (optarg) {
                    strcpy(exec_path, optarg);
                    exec_flag = 1;
                } else {
                    printf("%s", usage);
                    return -1;
                }
                break;
            case 5:
                if (optarg) {
                    if (with_binary(optarg) == -1) {
                        printf("%s", usage);
                        return -1;
                    }
                } else {
                    printf("%s", usage);
                    return -1;
                }
                break;
            default:
                printf("%s", usage);
                return -1;
        }
    }

    if (!exec_flag) {
        printf("%s", usage);
        return -1;
    }

    if (port_load_binary_from_file(exec_path, start_addr) == -1) {
        printf("%s", usage);
        return -1;
    }

    machine_start(start_addr, printreg);
}

static int with_binary(const char *data) {
    uint32_t addr;
    char path[64];
    sscanf(data, "%x#%s", &addr, path);
    return port_load_binary_from_file(path, addr);
}
