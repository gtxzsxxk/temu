//
// Created by hanyuan on 2024/4/23.
//
#include <stdio.h>
#include "parameters.h"
#include "mem.h"
#include "port/load_binary.h"

#if defined(WIN64) || defined(WIN32)
#include <windows.h>
#endif

int port_load_binary_from_file(const char *path, uint32_t addr) {
    int mem_ptr_flag;
    uint8_t *mem_ptr = pm_get_ptr(addr, &mem_ptr_flag);
    if (mem_ptr_flag == -1) {
        printf(TEMU_PRINT_BANNER"Failed to access memory at 0x%08x\r\n", addr);
        return -1;
    }

#if defined(WIN64) || defined(WIN32)
    DWORD file_read_size;
    HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf(TEMU_PRINT_BANNER"Failed to access %s\r\n", path);
        return -1;
    }
    DWORD dwFileSize = GetFileSize(hFile, NULL);
    if (!ReadFile(hFile, mem_ptr, dwFileSize, &file_read_size, NULL)) {
        printf(TEMU_PRINT_BANNER"Failed to read %s\r\n", path);
        CloseHandle(hFile);
        return -1;
    }
    CloseHandle(hFile);
    printf(TEMU_PRINT_BANNER"Read %lu bytes of %s.\n", file_read_size, path);
#else
    uint32_t file_read_size;
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        printf(TEMU_PRINT_BANNER"Failed to access %s\r\n", path);
        return -1;
    }
    file_read_size = fread(mem_ptr, mem_ptr_flag == MEM_PTR_RAM ? RAM_SIZE : ROM_SIZE - addr, 1, fp);
    fclose(fp);
    printf(TEMU_PRINT_BANNER"Read %u bytes of %s.\n", file_read_size, path);
#endif
    return 0;
}
