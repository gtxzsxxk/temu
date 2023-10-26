//
// Created by hanyuan on 2023/10/25.
//

#ifndef TEMU_MEMORY_H
#define TEMU_MEMORY_H

#include <types.h>
#include <cstdint>

class memory {
public:
    uint64_t *mem;

    /* The size is in MiB */
    memory(uint64_t size) {
        mem = new uint64_t[size * 65536];
    }

    ~memory() {
        delete[] mem;
    }
};


#endif //TEMU_MEMORY_H
