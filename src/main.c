//
// Created by hanyuan on 2024/2/8.
//
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include "machine.h"
#include "mem.h"

static const char *usage = "Usage: temu [-ram/-rom/-addr 0x02000000] [-printreg] -exec=program.bin -with=0x03000000!u-boot.bin\r\n";

static int load_binary(uint32_t addr, const char *path) {
    int mem_ptr_flag;
    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("Failed to access %s\r\n", path);
        return -1;
    }


    uint8_t *mem_ptr = mem_get_ptr(addr, &mem_ptr_flag);
    if (mem_ptr_flag == -1) {
        printf("Failed to access memory at 0x%08x\r\n", addr);
        return -1;
    }
    fread(mem_ptr, mem_ptr_flag == MEM_PTR_RAM ? RAM_SIZE : ROM_SIZE - addr, 1, fp);
    fclose(fp);
    return 0;
}

static int with_binary(const char *data) {
    uint32_t addr;
    char path[64];
    sscanf(data, "%x!%s", &addr, path);
    load_binary(addr, path);
}

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
                    printf(usage);
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
                    printf(usage);
                    return -1;
                }
                break;
            case 5:
                if (optarg) {
                    if (with_binary(optarg) == -1) {
                        printf(usage);
                        return -1;
                    }
                } else {
                    printf(usage);
                    return -1;
                }
                break;
        }
    }

    if (!exec_flag) {
        printf(usage);
        return -1;
    }

    if (load_binary(start_addr, exec_path) == -1) {
        printf(usage);
        return -1;
    }

    machine_start(start_addr, printreg);
    return 0;
}