//
// Created by hanyuan on 2024/5/6.
//

#include "tlb.h"
#include "zicsr.h"

static struct tlb_cache_line TLB[TLB_CACHE_LINE_SIZE][TLB_CACHE_WAY];

/* return the ppn of the virtual address */
uint32_t tlb_lookup(uint32_t vaddr, uint8_t access_flags, uint8_t *fault) {
    struct tlb_cache_line *indexed_line = TLB[TLB_VADDR_GET_INDEX(vaddr)];
    uint16_t tag = TLB_VADDR_GET_TAG(vaddr);
    uint8_t pgfault_flag = 0;
    for (uint8_t i = 0; i < TLB_CACHE_WAY; i++) {
        if (indexed_line[i].valid && indexed_line[i].tag == tag) {
            if (!((1 << (access_flags - 1)) & indexed_line[i].prot) ||
                (indexed_line[i].user_only && current_privilege != CSR_MASK_USER)) {
                /* TODO: check for user_only exceptions */
                pgfault_flag = 1;
                continue;
            }
            indexed_line[i].access_counter++;
            *fault = 0;
            return indexed_line[i].ppn;
        }
    }

    if (pgfault_flag) {
        *fault = TLB_PAGE_FAULT_IDENTIFIER;
        return 0xd1d1d1d1;
    }
    *fault = TLB_MISS_IDENTIFIER;
    return 0x3c3c3c3c;
}

void tlb_insert(uint32_t vaddr, struct tlb_cache_line data) {
    struct tlb_cache_line *indexed_line = TLB[TLB_VADDR_GET_INDEX(vaddr)];
    uint16_t min_used = 0xffff;
    struct tlb_cache_line *new_cache_line;
    for (uint8_t i = 0; i < TLB_CACHE_WAY; i++) {
        /* Cache未满 */
        if (!indexed_line[i].valid) {
            new_cache_line = &indexed_line[i];
            break;
        }

        /* Cache已满 */
        if (indexed_line[i].access_counter < min_used) {
            min_used = indexed_line[i].access_counter;
            new_cache_line = &indexed_line[i];
        }
    }

    *new_cache_line = data;
    new_cache_line->tag = TLB_VADDR_GET_TAG(vaddr);
    new_cache_line->valid = 1;
    new_cache_line->access_counter = 1;
}

void tlb_flushall() {
    for(uint32_t i = 0;i < TLB_CACHE_LINE_SIZE;i++){
        for(uint8_t j = 0; j < TLB_CACHE_WAY;j++) {
            TLB[i][j].valid = 0;
        }
    }
}
