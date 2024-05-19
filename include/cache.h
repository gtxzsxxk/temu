//
// Created by hanyuan on 2024/5/7.
//

#ifndef TEMU_CACHE_H
#define TEMU_CACHE_H

#include <stdint.h>

#define CACHE_INDEX_FIELD_LENGTH    7
#define CACHE_LINES                 (1 << CACHE_INDEX_FIELD_LENGTH)
#define ICACHE_WAYS_FIELD_LENGTH    1   /* 经过实验，此时效率最高 */
#define DCACHE_WAYS_FIELD_LENGTH    1
#define CACHE_GET_WAYS(x)           (1 << (x))
#define CACHE_OFFSET_FIELD_LENGTH   6
#define CACHE_LINE_DATA_SIZE        ((1 << CACHE_OFFSET_FIELD_LENGTH) >> 2)

#define CACHE_ADDR_GET_INDEX(x)     ((x >> CACHE_OFFSET_FIELD_LENGTH) & \
                                    (0xffffffff >> (32 - CACHE_INDEX_FIELD_LENGTH)))
#define CACHE_ADDR_GET_TAG(x)       ((x >> (CACHE_INDEX_FIELD_LENGTH + CACHE_OFFSET_FIELD_LENGTH)))


enum CACHE_DATA_SIZE {
    BYTE = 1,
    HalfWORD = 2,
    WORD = 4
};

/* 由于CPU，因此只能实现PIPT */

struct cache_line {
    uint32_t tag: (32 - CACHE_INDEX_FIELD_LENGTH - CACHE_OFFSET_FIELD_LENGTH);
    uint32_t data[CACHE_LINE_DATA_SIZE];
    uint16_t access_counter;
    uint8_t valid: 1;
    uint8_t dirty: 1;
};

uint8_t cache_data_read_b(uint32_t paddr, uint8_t *intr);

uint16_t cache_data_read_h(uint32_t paddr, uint8_t *intr);

uint32_t cache_data_read_w(uint32_t paddr, uint8_t *intr);

uint32_t cache_inst_read(uint32_t paddr, uint8_t *intr);

void cache_data_write_b(uint32_t paddr, uint8_t data, uint8_t *intr);

void cache_data_write_h(uint32_t paddr, uint16_t data, uint8_t *intr);

void cache_data_write_w(uint32_t paddr, uint32_t data, uint8_t *intr);

void cache_flush_icache();

#endif //TEMU_CACHE_H
