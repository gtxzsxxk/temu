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
    fread(&hdr, sizeof(hdr), 1, fp);
    Elf64_Shdr *section_headers = new Elf64_Shdr[hdr.e_shnum];
    fseek(fp, hdr.e_shoff, SEEK_SET);
    fread(section_headers, sizeof(Elf64_Shdr) * hdr.e_shnum, 1, fp);
    fclose(fp);

    delete[] section_headers;
}
