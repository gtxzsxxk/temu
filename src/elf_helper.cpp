//
// Created by hanyuan on 2023/10/25.
//
#include <iostream>
#include "elf_helper.h"

void elf_helper::read_elf() {
    auto *fp = fopen(elf_file_path.c_str(), "r");
    if (fp == nullptr) {
        std::cerr << "Cannot open file: " << elf_file_path << std::endl;
        return;
    }
    Elf64_Ehdr hdr;
    fread(&hdr,sizeof(hdr),1,fp);
    fclose(fp);
}
