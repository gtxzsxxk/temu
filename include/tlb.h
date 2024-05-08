//
// Created by hanyuan on 2024/5/6.
//

#ifndef TEMU_TLB_H
#define TEMU_TLB_H

#include <stdint.h>

#define TLB_CACHE_LINE_FIELD_LENGTH     10
#define TLB_CACHE_LINE_SIZE             (1 << TLB_CACHE_LINE_FIELD_LENGTH)
#define SV32_PAGE_OFFSET_FIELD_LEN      12
#define SV32_VPN_FIELD_LENGTH           (32 - SV32_PAGE_OFFSET_FIELD_LEN)
#define TLB_CACHE_TAG_FIELD_LENGTH      (SV32_VPN_FIELD_LENGTH - TLB_CACHE_LINE_FIELD_LENGTH)
#define TLB_CACHE_WAY                   1   /* 由于大小限制，退化为直接映射 */

#define TLB_MISS_IDENTIFIER             1
#define TLB_PAGE_FAULT_IDENTIFIER       2

#define TLB_VADDR_GET_INDEX(x)      ((x >> SV32_PAGE_OFFSET_FIELD_LEN) & (0xffffffff >> (32 - TLB_CACHE_LINE_FIELD_LENGTH)))
#define TLB_VADDR_GET_TAG(x)        (x >> (SV32_PAGE_OFFSET_FIELD_LEN + TLB_CACHE_LINE_FIELD_LENGTH))

/* 组相联 */
struct tlb_cache_line {
    uint16_t tag: TLB_CACHE_TAG_FIELD_LENGTH;
    uint16_t access_counter;
    uint32_t ppn: SV32_VPN_FIELD_LENGTH;
    uint8_t valid: 1;
    uint8_t prot: 3;
    uint8_t user_only: 1;
};

uint32_t tlb_lookup(uint32_t vaddr, uint8_t access_flags, uint8_t *fault);

void tlb_insert(uint32_t vaddr, struct tlb_cache_line data);

void tlb_flushall();

#endif //TEMU_TLB_H
