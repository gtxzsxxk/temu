//
// Created by hanyuan on 2023/10/27.
//
#include <cstring>
#include "cpu.h"

void cpu::load_elf(elf_helper &handler) {
    auto sec_info = handler.get_section_headers();
    auto *sec_hdr = sec_info.first;
    uint8_t sec_num = sec_info.second;
    auto fp = handler.get_elf_fp();
    for (int i = 0; i < sec_num; i++) {
        Elf64_Shdr sec = *(sec_hdr + i);
        uint64_t *addr = memory_map((uint64_t *) sec.sh_addr);
        if (addr == nullptr) {
            continue;
        }
        fseek(fp, sec.sh_offset, SEEK_SET);
        fread(addr, sec.sh_size, 1, fp);
    }
    entry_addr = handler.get_header().e_entry;
    auto strtlb_sec_hdr = sec_hdr[handler.get_header().e_shstrndx];
    char *str_buffer = new char[1024];
    fseek(fp, strtlb_sec_hdr.sh_offset, SEEK_SET);
    fread(str_buffer, strtlb_sec_hdr.sh_size, 1, fp);
    /* TODO: move here to the machine's definition */
    for (int i = 0; i < sec_num; i++) {
        if (!strcmp(str_buffer + sec_hdr[i].sh_name, "._user_heap_stack")) {
            registers->write(2, sec_hdr[i].sh_addr);
        }
        if (!strcmp(str_buffer + sec_hdr[i].sh_name, ".text")) {
            text_size = sec_hdr[i].sh_size;
        }
        if (!strcmp(str_buffer + sec_hdr[i].sh_name, ".bss")) {
            registers->write(3, sec_hdr[i].sh_addr);
        }
    }
    delete[] str_buffer;
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
