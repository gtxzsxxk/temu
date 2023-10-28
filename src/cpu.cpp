//
// Created by hanyuan on 2023/10/27.
//
#include "cpu.h"

void cpu::load_elf(elf_helper &handler) {
    auto sec_info = handler.get_section_headers();
    auto *sec_hdr = sec_info.first;
    uint8_t sec_num = sec_info.second;
    for (int i = 0; i < sec_num; i++) {
        Elf64_Shdr sec = *(sec_hdr + i);
        uint64_t *addr = memory_map((uint64_t *) sec.sh_addr);
        if (addr == nullptr) {
            continue;
        }
        auto fp = handler.get_elf_fp();
        fseek(fp, sec.sh_offset, SEEK_SET);
        fread(addr, sec.sh_size, 1, fp);
    }
    entry_addr = handler.get_header().e_entry;
}

uint64_t *cpu::memory_map(uint64_t *mem) {
    uint64_t *tmp;
    tmp = RAM.remap(mem);
    if (tmp != nullptr) {
        return tmp;
    }
    tmp = ROM.remap(mem);
    if (tmp != nullptr) {
        return tmp;
    }
    return nullptr;
}

register_file *cpu::get_registers() const {
    return registers;
}
