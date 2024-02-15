//
// Created by hanyuan on 2024/2/8.
//
#include <stdio.h>
#include "machine.h"
#include "mem.h"


int main(int argc, char **argv){
    if(argc == 2){
        char *path = argv[1];
        FILE *fp = fopen(path,"r");
        if(!fp) {
            return -1;
        }
        fread(mem_get_rom_ptr(),ROM_SIZE,1,fp);
        fclose(fp);
        machine_start();
        return 0;
    }
    return -1;
}