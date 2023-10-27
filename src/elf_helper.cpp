//
// Created by hanyuan on 2023/10/25.
//
#include <iostream>
#include "elf_helper.h"

void elf_helper::read_elf() {
    fp = fopen(elf_file_path.c_str(), "r");
    if (fp == nullptr) {
        std::cerr << "Cannot open file: " << elf_file_path << std::endl;
        return;
    }
    fread(&hdr, sizeof(hdr), 1, fp);
    section_headers = new Elf64_Shdr[hdr.e_shnum];
    fseek(fp, hdr.e_shoff, SEEK_SET);
    fread(section_headers, sizeof(Elf64_Shdr) * hdr.e_shnum, 1, fp);

    delete[] section_headers;
}

std::pair<Elf64_Shdr *, uint8_t> elf_helper::get_section_headers() const {
    return std::make_pair(section_headers, hdr.e_shnum);
}

elf_helper::~elf_helper() {
    fclose(fp);
}

FILE *elf_helper::get_elf_fp() const {
    return fp;
}

Elf64_Ehdr elf_helper::get_header() const {
    return hdr;
}
