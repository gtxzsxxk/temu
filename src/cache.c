//
// Created by hanyuan on 2024/5/7.
//

#include "cache.h"
#include "parameters.h"
#include "port/main_memory.h"

static struct cache_line ICACHE[CACHE_LINES][CACHE_WAYS] = {0};
static struct cache_line DCACHE[CACHE_LINES][CACHE_WAYS] = {0};

#ifndef NULL
#define NULL (void*)0
#endif

static uint32_t physical_memory_read_w(uint32_t addr, uint8_t *intr);

static void physical_memory_write_w(uint32_t addr, uint32_t data, uint8_t *intr);

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
    uint16_t offset = (paddr & (0xffffffff >> (32 - CACHE_OFFSET_FIELD_LENGTH)));
    for (uint8_t i = 0; i < CACHE_WAYS; i++) {
        if (cacheline_set[i].valid && cacheline_set[i].tag == tag) {
            cacheline_set[i].access_counter++;
            *miss = 0;
            cacheline_set[i].dirty = 1;
            switch (datasize) {
                case BYTE:
                    *(((uint8_t *) cacheline_set[i].data) + offset) = data_b;
                    return;
                case HalfWORD:
                    *((uint16_t *) (((uint8_t *) cacheline_set[i].data) + offset)) = data_h;
                    return;
                case WORD:
                    *((uint32_t *) (((uint8_t *) cacheline_set[i].data) + offset)) = data_w;
                    return;
            }
        }
    }

    *miss = 1;
}

static inline void
cache_load(struct cache_line cache[CACHE_LINES][CACHE_WAYS], uint32_t paddr, uint8_t *load_fault) {
    struct cache_line *cacheline_set = cache[CACHE_ADDR_GET_INDEX(paddr)];
    uint32_t tag = CACHE_ADDR_GET_TAG(paddr);
    uint16_t min_used = 0xffff;
    struct cache_line *new_cache_line = NULL;
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

    if (new_cache_line->valid && new_cache_line->dirty) {
        /* Write Back */
        uint32_t wb_addr = (CACHE_ADDR_GET_INDEX(paddr) << CACHE_OFFSET_FIELD_LENGTH) |
                           (new_cache_line->tag << (CACHE_OFFSET_FIELD_LENGTH + CACHE_INDEX_FIELD_LENGTH));
        for (uint32_t i = 0; i < CACHE_LINE_DATA_SIZE; i++) {
            physical_memory_write_w(wb_addr + (i << 2), new_cache_line->data[i], NULL);
        }
    }

    new_cache_line->tag = tag;
    new_cache_line->access_counter = 1;
    new_cache_line->valid = 1;
    new_cache_line->dirty = 0;
    uint32_t base_addr = (paddr >> CACHE_OFFSET_FIELD_LENGTH) << CACHE_OFFSET_FIELD_LENGTH;
    for (uint32_t i = 0; i < CACHE_LINE_DATA_SIZE; i++) {
        new_cache_line->data[i] = physical_memory_read_w(base_addr + (i << 2), load_fault);
        if (load_fault && *load_fault) {
            return;
        }
    }
}

uint8_t cache_data_read_b(uint32_t paddr, uint8_t *intr) {
    uint8_t miss = 0;
    uint32_t data = cache_read(DCACHE, paddr, &miss);
    if (!miss) {
        return *((uint8_t *) &data + (paddr & 0x03));
    } else {
        cache_load(DCACHE, paddr, intr);
        if (intr && *intr) {
            return 0x12;
        }
        return cache_data_read_b(paddr, intr);
    }
}

uint16_t cache_data_read_h(uint32_t paddr, uint8_t *intr) {
    uint8_t miss = 0;
    uint32_t data = cache_read(DCACHE, paddr, &miss);
    if (!miss) {
        return *((uint16_t *) ((uint8_t *) &data + (paddr & 0x03)));
    } else {
        cache_load(DCACHE, paddr, intr);
        if (*intr) {
            return 0x3412;
        }
        return cache_data_read_h(paddr, intr);
    }
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

uint32_t cache_inst_read(uint32_t paddr, uint8_t *intr) {
    uint8_t miss = 0;
    uint32_t data = cache_read(ICACHE, paddr, &miss);
    if (!miss) {
        return data;
    } else {
        cache_load(ICACHE, paddr, intr);
        if (*intr) {
            return 0x51515151;
        }
        return cache_inst_read(paddr, intr);
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

void cache_flush_icache() {
    for (uint32_t i = 0; i < CACHE_LINES; i++) {
        for (uint8_t j = 0; j < CACHE_WAYS; j++) {
            *(((uint32_t *) (&ICACHE[i][j])) + 17) = 0;
            if (DCACHE[i][j].valid && DCACHE[i][j].dirty) {
                /* Write Back */
                uint32_t wb_addr = (i << CACHE_OFFSET_FIELD_LENGTH) |
                                   (DCACHE[i][j].tag << (CACHE_OFFSET_FIELD_LENGTH + CACHE_INDEX_FIELD_LENGTH));
                for (uint32_t k = 0; k < CACHE_LINE_DATA_SIZE; k++) {
                    physical_memory_write_w(wb_addr + (k << 2), DCACHE[i][j].data[k], NULL);
                }
                DCACHE[i][j].dirty = 0;
            }
        }
    }
}

static uint32_t physical_memory_read_w(uint32_t addr, uint8_t *intr) {
    if (addr % 4) {
        if (intr) {
            *intr = 3;
        }
        return 0xff;
    }
    if (addr >= RAM_BASE_ADDR && addr + 3 < RAM_BASE_ADDR + RAM_SIZE) {
        return port_main_memory_read_w(addr - RAM_BASE_ADDR);
    } else {
        if (intr) {
            *intr = 1;
        }

        return 0x6666ffff;
    }
}

static void physical_memory_write_w(uint32_t addr, uint32_t data, uint8_t *intr) {
    if (addr % 4) {
        if (intr) {
            *intr = 3;
        }
    }

    if (addr >= RAM_BASE_ADDR && addr + 3 < RAM_BASE_ADDR + RAM_SIZE) {
        port_main_memory_write_w(addr - RAM_BASE_ADDR, data);
    } else {
        if (intr) {
            *intr = 1;
        }
    }
}
