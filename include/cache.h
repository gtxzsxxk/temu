//
// Created by hanyuan on 2024/5/7.
//

#ifndef TEMU_CACHE_H
#define TEMU_CACHE_H
#include <stdint.h>

#define CACHE_INDEX_FIELD_LENGTH    10
#define CACHE_OFFSET_FIELD_LENGTH   12
#define CACHE_LINE_DATA_SIZE        16

/* 由于CPU，因此只能实现PIPT */

struct cache_line {
    uint16_t tag: (32 - CACHE_INDEX_FIELD_LENGTH - CACHE_OFFSET_FIELD_LENGTH);
    uint16_t access_counter;
    uint32_t data[CACHE_LINE_DATA_SIZE];
    uint8_t valid: 1;
};

#endif //TEMU_CACHE_H
