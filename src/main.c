//
// Created by hanyuan on 2024/2/8.
//
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include "machine.h"
#include "mem.h"


int main(int argc, char **argv) {
    /* Usage: temu [-ram/-rom/-addr 0x02000000] [-printreg] -exec=program.bin */
    uint32_t start_addr = 0;
    int opt_index, mem_ptr_flag, exec_flag = 0, printreg = 0;
    char exec_path[64] = {0};

    struct option opts[] = {
            {"rom",      no_argument,       0, 0},
            {"ram",      no_argument,       0, 0},
            {"addr",     required_argument, 0, 0},
            {"printreg", no_argument,       0, 0},
            {"exec",     required_argument, 0, 0},
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
                    printf("Usage: temu [-ram/-rom/-addr 0x02000000] [-printreg] -exec=program.bin\r\n");
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
                    printf("Usage: temu [-ram/-rom/-addr 0x02000000] [-printreg] -exec=program.bin\r\n");
                    return -1;
                }
                break;
        }
    }

    if (!exec_flag) {
        printf("Usage: temu [-ram/-rom/-addr 0x02000000] [-printreg] -exec=program.bin\r\n");
        return -1;
    }

    FILE *fp = fopen(exec_path, "r");
    if (!fp) {
        printf("Failed to access %s\r\n", exec_path);
        return -1;
    }


    uint8_t *mem_ptr = mem_get_ptr(start_addr, &mem_ptr_flag);
    if (mem_ptr_flag == -1) {
        printf("Failed to access memory at 0x%08x\r\n", start_addr);
        return -1;
    }
    fread(mem_ptr, mem_ptr_flag == MEM_PTR_RAM ? RAM_SIZE : ROM_SIZE - start_addr, 1, fp);
    fclose(fp);
    machine_start(start_addr);
    return 0;
}