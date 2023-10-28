//
// Created by hanyuan on 2023/10/25.
//

#ifndef TEMU_MEMORY_H
#define TEMU_MEMORY_H

#include <types.h>
#include <cstdint>

class memory {
public:
    uint64_t base_addr;
    uint64_t *mem;
    uint64_t length_bytes;

    /* The size is in MiB */
    memory(uint64_t base_addr, uint64_t size) : base_addr(base_addr) {
        length_bytes = size * 1024 * 1024;
        mem = new uint64_t[length_bytes >> 3];
    }

    ~memory() {
        delete[] mem;
    }

    uint64_t *remap(uint64_t *from) const {
        uint64_t cmp = (uint64_t) from;
        if (cmp >= base_addr) {
            uint64_t diff = cmp - base_addr;
            if (diff <= length_bytes) {
                return (uint64_t *) ((uint8_t *) mem + diff);
            }
        }
        return nullptr;
    }
};


#endif //TEMU_MEMORY_H
