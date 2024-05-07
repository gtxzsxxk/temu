//
// Created by hanyuan on 2024/5/7.
//

#include "parameters.h"
#include "cache.h"

static struct cache_line ICACHE[ICACHE_LINES][ICACHE_WAYS];
static struct cache_line DCACHE[DCACHE_LINES][DCACHE_WAYS];

uint32_t cache_read(struct cache_line** cache, uint32_t paddr) {
    struct cache_line* cacheline_set = cache[]
}
