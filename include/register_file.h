//
// Created by hanyuan on 2023/10/27.
//

#ifndef TEMU_REGISTER_FILE_H
#define TEMU_REGISTER_FILE_H

#include <iostream>
#include <iomanip>
#include <cstdint>

class register_file {
    uint64_t *registers;
    const uint64_t register_len;
public:
    uint64_t program_counter = 0;

    register_file(uint64_t register_len) : register_len(register_len) {
        registers = new uint64_t[register_len];
        for (int i = 0; i < register_len; i++) {
            registers[i] = 0;
        }
    }

    void write(uint64_t rd, uint64_t value) {
        registers[rd] = value;
    }

    uint64_t read(uint64_t rd) {
        if (rd == 0) {
            return 0;
        }
        return registers[rd];
    }

    void debug_print() {
        std::cout << "Register File:" << std::endl;
        for (int i = 0; i < register_len; i++) {
            std::cout << "Register X" << std::dec << i << ": 0x" << std::setw(16) << std::setfill('0') << std::hex
                      << read(i)
                      << std::endl;
        }
        std::cout << "Program Counter" << ": 0x" << std::setw(16) << std::setfill('0') << std::hex
                  << program_counter
                  << std::endl;
        std::cout << std::endl;
    }

    ~register_file() {
        delete[] registers;
    }
};


#endif //TEMU_REGISTER_FILE_H
