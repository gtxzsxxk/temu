//
// Created by hanyuan on 2024/5/7.
//

#include "cache.h"
#include "parameters.h"
#include "vm.h"

static struct cache_line ICACHE[CACHE_LINES][CACHE_WAYS];
static struct cache_line DCACHE[CACHE_LINES][CACHE_WAYS];

#ifndef NULL
#define NULL (void*)0
#endif

/* the physical address needs to be aligned with 4 bytes */
static inline uint32_t cache_read(struct cache_line cache[CACHE_LINES][CACHE_WAYS], uint32_t paddr, uint8_t *miss) {
    struct cache_line *cacheline_set = cache[CACHE_ADDR_GET_INDEX(paddr)];
    uint32_t tag = CACHE_ADDR_GET_TAG(paddr);
    uint16_t offset = (paddr & (0xffffffff >> (32 - CACHE_OFFSET_FIELD_LENGTH))) >> 2;
    for (uint8_t i = 0; i < CACHE_WAYS; i++) {
        if (cacheline_set[i].valid && cacheline_set[i].tag == tag) {
            cacheline_set[i].access_counter++;
            *miss = 0;
            return cacheline_set[i].data[offset];
        }
    }

    *miss = 1;
    return 0x30303030;
}

static inline void cache_write(struct cache_line cache[CACHE_LINES][CACHE_WAYS], uint32_t paddr, uint8_t data_b,
                        uint16_t data_h, uint32_t data_w, enum CACHE_DATA_SIZE datasize, uint8_t *miss) {
    struct cache_line *cacheline_set = cache[CACHE_ADDR_GET_INDEX(paddr)];
    uint32_t tag = CACHE_ADDR_GET_TAG(paddr);
    uint16_t offset = (paddr & (0xffffffff >> (32 - CACHE_OFFSET_FIELD_LENGTH))) >> 2;
    for (uint8_t i = 0; i < CACHE_WAYS; i++) {
        if (cacheline_set[i].valid && cacheline_set[i].tag == tag) {
            cacheline_set[i].access_counter++;
            *miss = 0;
            cacheline_set[i].dirty = 1;
            switch (datasize) {
                case BYTE:
                    *((uint8_t *) &cacheline_set[i].data[offset] + (paddr & 0x03)) = data_b;
                    return;
                case HalfWORD:
                    *((uint16_t *) &cacheline_set[i].data[offset] + ((paddr >> 1) & 0x01)) = data_h;
                    return;
                case WORD:
                    cacheline_set[i].data[offset] = data_w;
                    return;
            }
        }
    }

    *miss = 1;
}

static inline void cache_load(struct cache_line cache[CACHE_LINES][CACHE_WAYS], uint32_t paddr, uint8_t *load_fault) {
    struct cache_line *cacheline_set = cache[CACHE_ADDR_GET_INDEX(paddr)];
    uint32_t tag = CACHE_ADDR_GET_TAG(paddr);
    uint16_t min_used = 0xffff;
    struct cache_line *new_cache_line;
    for (uint8_t i = 0; i < CACHE_WAYS; i++) {
        if (!cacheline_set[i].valid) {
            new_cache_line = &cacheline_set[i];
            break;
        }

        if (cacheline_set[i].access_counter < min_used) {
            min_used = cacheline_set[i].access_counter;
            new_cache_line = &cacheline_set[i];
        }
    }

    if (new_cache_line->dirty) {
        /* Write Back */
        uint32_t wb_addr = (CACHE_ADDR_GET_INDEX(paddr) << (32 - CACHE_INDEX_FIELD_LENGTH)) |
                           (new_cache_line->tag << CACHE_OFFSET_FIELD_LENGTH);
        for (uint32_t i = 0; i < CACHE_LINE_DATA_SIZE; i++) {
            pm_write_w(wb_addr + (i << 2), new_cache_line->data[i], NULL);
        }
    }

    new_cache_line->tag = tag;
    new_cache_line->access_counter = 1;
    new_cache_line->valid = 1;
    new_cache_line->dirty = 0;
    uint32_t base_addr = (paddr >> CACHE_OFFSET_FIELD_LENGTH) << CACHE_OFFSET_FIELD_LENGTH;
    for (uint32_t i = 0; i < CACHE_LINE_DATA_SIZE; i++) {
        new_cache_line->data[i] = pm_read_w(base_addr + (i << 2), load_fault);
        if (*load_fault) {
            return;
        }
    }
}

uint8_t cache_data_read_b(uint32_t paddr, uint8_t *intr) {
    uint8_t miss = 0;
    uint32_t data = cache_read(DCACHE, paddr, &miss);
    if (!miss) {
        switch (paddr & 0x03) {
            case 0:
                return data;
            case 1:
                return data >> 8;
            case 2:
                return data >> 16;
            case 3:
                return data >> 24;
        }
    } else {
        cache_load(DCACHE, paddr, intr);
        if (*intr) {
            return 0x12;
        }
        return cache_data_read_b(paddr, intr);
    }

    return 0x12;
}

uint16_t cache_data_read_h(uint32_t paddr, uint8_t *intr) {
    uint8_t miss = 0;
    uint32_t data = cache_read(DCACHE, paddr, &miss);
    if (!miss) {
        switch ((paddr >> 1) & 0x01) {
            case 0:
                return data;
            case 1:
                return data >> 16;
        }
    } else {
        cache_load(DCACHE, paddr, intr);
        if (*intr) {
            return 0x3412;
        }
        return cache_data_read_h(paddr, intr);
    }
    return 0x3412;
}

uint32_t cache_data_read_w(uint32_t paddr, uint8_t *intr) {
    uint8_t miss = 0;
    uint32_t data = cache_read(DCACHE, paddr, &miss);
    if (!miss) {
        return data;
    } else {
        cache_load(DCACHE, paddr, intr);
        if (*intr) {
            return 0x51515151;
        }
        return cache_data_read_w(paddr, intr);
    }
}

uint32_t cache_data_read_inst(uint32_t paddr, uint8_t *intr) {
    uint8_t miss = 0;
    uint32_t data = cache_read(ICACHE, paddr, &miss);
    if (!miss) {
        return data;
    } else {
        cache_load(ICACHE, paddr, intr);
        if (*intr) {
            return 0x51515151;
        }
        return cache_data_read_inst(paddr, intr);
    }
}

void cache_data_write_b(uint32_t paddr, uint8_t data, uint8_t *intr) {
    uint8_t miss = 0;
    cache_write(DCACHE, paddr, data, 0, 0, BYTE, &miss);
    if (miss) {
        cache_load(DCACHE, paddr, intr);
        if (*intr) {
            return;
        }
        cache_data_write_b(paddr, data, intr);
    }
}

void cache_data_write_h(uint32_t paddr, uint16_t data, uint8_t *intr) {
    uint8_t miss = 0;
    cache_write(DCACHE, paddr, 0, data, 0, HalfWORD, &miss);
    if (miss) {
        cache_load(DCACHE, paddr, intr);
        if (*intr) {
            return;
        }
        cache_data_write_h(paddr, data, intr);
    }
}

void cache_data_write_w(uint32_t paddr, uint32_t data, uint8_t *intr) {
    uint8_t miss = 0;
    cache_write(DCACHE, paddr, 0, 0, data, WORD, &miss);
    if (miss) {
        cache_load(DCACHE, paddr, intr);
        if (*intr) {
            return;
        }
        cache_data_write_w(paddr, data, intr);
    }
}
