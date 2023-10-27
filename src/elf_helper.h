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
    FILE *fp{};
    Elf64_Ehdr hdr{};
    Elf64_Shdr *section_headers{};

    void read_elf();

public:
    explicit elf_helper(std::string elf_file_path) : elf_file_path(std::move(elf_file_path)) {
        read_elf();
    }

    [[nodiscard]] std::pair<Elf64_Shdr *, uint8_t> get_section_headers() const;

    FILE *get_elf_fp() const;

    ~elf_helper();
};


#endif //TEMU_ELF_HELPER_H
