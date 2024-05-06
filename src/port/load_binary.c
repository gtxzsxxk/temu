//
// Created by hanyuan on 2024/4/23.
//
#include <stdio.h>
#include "parameters.h"
#include "port/main_memory.h"
#include "port/load_binary.h"

#if defined(WIN64) || defined(WIN32)
#include <windows.h>
#endif

int port_load_binary_from_file(const char *path, uint32_t addr) {
    uint8_t load_buffer[256];
    uint32_t main_memory_offset = addr - (uint32_t)RAM_BASE_ADDR;

#if defined(WIN64) || defined(WIN32)
#error Not port the load binary function to this platform!
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
    uint32_t file_read_size = 0;
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        printf(TEMU_PRINT_BANNER"Failed to access %s\n", path);
        return -1;
    }
    while (!feof(fp)) {
        uint32_t read_n = fread(load_buffer, 1, 256, fp);
        for (uint32_t i = 0; i < read_n; i++) {
            port_main_memory_write(main_memory_offset, load_buffer[i]);
            main_memory_offset++;
        }
        file_read_size += read_n;
    }
    fclose(fp);
    printf(TEMU_PRINT_BANNER"Write %u bytes of %s to 0x%8x -- 0x%x.\n", file_read_size, path, addr, main_memory_offset);
#endif
    return 0;
}
