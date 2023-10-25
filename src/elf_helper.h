//
// Created by hanyuan on 2023/10/25.
//

#ifndef TEMU_ELF_HELPER_H
#define TEMU_ELF_HELPER_H

#include <types.h>
#include <string>
#include "elf.h"

class elf_helper {
    std::string elf_file_path;
    uint64_t entry_addr;

    void read_elf();

public:
    elf_helper(std::string elf_file_path) : elf_file_path(elf_file_path) {
        read_elf();
    }
};


#endif //TEMU_ELF_HELPER_H
