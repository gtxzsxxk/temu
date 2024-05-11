//
// Created by hanyuan on 2024/5/6.
//

#include "tlb.h"
#include "mmu.h"
#include "zicsr.h"

#ifndef NULL
#define NULL (void*)0
#endif

static struct tlb_cache_line TLB[TLB_CACHE_LINE_SIZE][TLB_CACHE_WAY];

/* return the ppn of the virtual address */
uint32_t tlb_lookup(uint32_t vaddr, uint8_t access_flags, uint8_t *fault) {
    struct tlb_cache_line *cacheline_set = TLB[TLB_VADDR_GET_INDEX(vaddr)];
    uint16_t tag = TLB_VADDR_GET_TAG(vaddr);
    uint8_t pgfault_flag = 0;
    for (uint8_t i = 0; i < TLB_CACHE_WAY; i++) {
        if (cacheline_set[i].valid && cacheline_set[i].tag == tag) {
            if (!((1 << (access_flags - 1)) & cacheline_set[i].prot) ||
                (cacheline_set[i].user_only && current_privilege == CSR_MASK_SUPERVISOR && !vm_status_read_sum())) {
                pgfault_flag = 1;
                continue;
            }
            cacheline_set[i].access_counter++;
            *fault = 0;
            return cacheline_set[i].ppn;
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
    struct tlb_cache_line *cacheline_set = TLB[TLB_VADDR_GET_INDEX(vaddr)];
    uint16_t min_used = 0xffff;
    struct tlb_cache_line *new_cache_line = NULL;
    for (uint8_t i = 0; i < TLB_CACHE_WAY; i++) {
        /* Cache未满 */
        if (!cacheline_set[i].valid) {
            new_cache_line = &cacheline_set[i];
            break;
        }

        /* Cache已满 */
        if (cacheline_set[i].access_counter < min_used) {
            min_used = cacheline_set[i].access_counter;
            new_cache_line = &cacheline_set[i];
        }
    }

    new_cache_line->ppn = data.ppn;
    new_cache_line->prot = data.prot;
    new_cache_line->user_only = data.user_only;
    new_cache_line->tag = TLB_VADDR_GET_TAG(vaddr);
    new_cache_line->valid = 1;
    new_cache_line->access_counter = 1;
}

void tlb_flushall() {
    for (uint32_t i = 0; i < TLB_CACHE_LINE_SIZE; i++) {
        for (uint8_t j = 0; j < TLB_CACHE_WAY; j++) {
            *(((uint32_t *) &TLB[i][j]) + 1) = 0;
        }
    }
}
