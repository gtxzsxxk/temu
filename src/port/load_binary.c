//
// Created by hanyuan on 2024/4/23.
//
#include "parameters.h"
#include "port/main_memory.h"
#include "port/load_binary.h"

/// 从文件中加载二进制，并且装入主存中
/// \param path 二进制文件在文件系统中的路径
/// \param addr 主存中的目标地址
/// \return 0 for success
int port_load_binary_from_file(const char *path, uint32_t addr) {
    uint8_t load_buffer[256];
    uint32_t main_memory_offset = addr - (uint32_t) RAM_BASE_ADDR;
    uint32_t file_read_size = 0;
    FILE *fp = PORT_FILE_OPEN(path, "rb");
    if (!fp) {
        printf(TEMU_PRINT_BANNER"Failed to access %s\n", path);
        return -1;
    }
    while (!PORT_FILE_EOF(fp)) {
        uint32_t read_n = PORT_FILE_READ(load_buffer, 1, 256, fp);
        for (uint32_t i = 0; i < read_n; i++) {
            port_main_memory_load_b(main_memory_offset, load_buffer[i]);
            main_memory_offset++;
        }
        file_read_size += read_n;
    }
    PORT_FILE_CLOSE(fp);
#ifndef BARE_METAL_PLATFORM
    printf(TEMU_PRINT_BANNER"Write %u bytes of %s to 0x%8x -- 0x%x.\n", file_read_size, path, addr, main_memory_offset);
#endif
    return 0;
}
