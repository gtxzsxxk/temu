#include <iostream>
#include <unistd.h>

#include "elf_helper.h"

int main(int argc, char **argv) {
    int opt;
    const char *optstring = "e";
    std::string elf_path;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        elf_path = std::move(std::string(argv[optind]));
    }
    if (std::empty(elf_path)) {
        std::cerr << "No ELF file given." << std::endl;
        return -1;
    }
    auto elf_handler = elf_helper(elf_path);
    return 0;
}
