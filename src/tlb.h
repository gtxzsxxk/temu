//
// Created by hanyuan on 2024/5/6.
//

#ifndef TEMU_TLB_H
#define TEMU_TLB_H
#include <stdint.h>

#define TLB_CACHE_LINE_SIZE_LOG     10
#define SV32_VPN_FIELD_LENGTH       20
#define TLB_CACHE_WAY               2

/* 组相联 */
struct tlb_cache_line {
    uint16_t tag:(SV32_VPN_FIELD_LENGTH - TLB_CACHE_LINE_SIZE_LOG);
    uint32_t ppn:SV32_VPN_FIELD_LENGTH;
    uint8_t valid:1;
    uint8_t prot:3;
    uint8_t user_only:1;

};

#endif //TEMU_TLB_H
