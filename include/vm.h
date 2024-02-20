//
// Created by hanyuan on 2024/2/20.
//

#ifndef TEMU_VM_H
#define TEMU_VM_H

#define MEM_PTR_ROM         1
#define MEM_PTR_RAM         2

uint8_t vm_on(void);

uint8_t *pm_get_ptr(uint32_t addr, int *ok_flag);

uint8_t pm_read_b(uint32_t addr, uint8_t *intr);

uint16_t pm_read_h(uint32_t addr, uint8_t *intr);

uint32_t pm_read_w(uint32_t addr, uint8_t *intr);

void pm_write_b(uint32_t addr, uint8_t data, uint8_t *intr);

void pm_write_h(uint32_t addr, uint16_t data, uint8_t *intr);

void pm_write_w(uint32_t addr, uint32_t data, uint8_t *intr);

#endif //TEMU_VM_H
