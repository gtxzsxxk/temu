cmake_minimum_required(VERSION 3.22)
project(temu C)

set(CMAKE_C_STANDARD 11)

# Download info
set(URL "https://cloud.tsinghua.edu.cn/f/b639b1cc87784504aab4/?dl=1")
set(DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/../linux")
set(FILE_NAME "binary.zip")
file(MAKE_DIRECTORY ${DOWNLOAD_DIR})
set(LOCAL_FILE_PATH "${DOWNLOAD_DIR}/${FILE_NAME}")

set(PerformanceMode ON)
set(PerformanceMonitor OFF)
if (PerformanceMode)
    message(STATUS "${CMAKE_C_COMPILER_ID} toolchain.")
    if ("${CMAKE_C_COMPILER_ID}" MATCHES "^(Apple)?Clang$")
        message(STATUS "Using -O3.")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    else ()
        message(STATUS "Using -O2.")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -fno-strict-aliasing")
    endif ()
    message(STATUS "Compiling with performance mode")
else ()
    message(STATUS "Compiling with NO performance")
endif ()
if (NOT PerformanceMonitor)
    add_definitions(-DPERF_MUTE_ALL)
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g")

include_directories(include)

add_executable(temu src/main.c
        src/mmu.c
        src/machine.c
        src/decode.c
        src/zicsr.c
        include/zicsr.h
        src/trap.c
        include/trap.h
        include/uart8250.h
        src/peripherals/uart8250.c
        src/peripherals/plic-simple.c
        include/plic-simple.h
        src/port/lock.c
        include/port/lock.h
        src/port/load_binary.c
        include/port/load_binary.h
        src/port/console.c
        include/port/console.h
        include/perf.h
        include/port/main_memory.h
        src/port/main_memory.c
        src/tlb.c
        include/tlb.h
        src/cache.c
        include/cache.h
        src/port/system_timer.c
        include/port/system_timer.h
        src/port/port_main.c
        include/port/port_main.h
        src/port/os_yield_cpu.c
        include/port/os_yield_cpu.h)

add_library(temudifftest SHARED src/main.c
        src/mmu.c
        src/machine.c
        src/decode.c
        src/zicsr.c
        include/zicsr.h
        src/trap.c
        include/trap.h
        include/uart8250.h
        src/peripherals/uart8250.c
        src/peripherals/plic-simple.c
        include/plic-simple.h
        src/port/lock.c
        include/port/lock.h
        src/port/load_binary.c
        include/port/load_binary.h
        src/port/console.c
        include/port/console.h
        include/perf.h
        include/port/main_memory.h
        src/port/main_memory.c
        src/tlb.c
        include/tlb.h
        src/cache.c
        include/cache.h
        src/port/system_timer.c
        include/port/system_timer.h
        src/port/port_main.c
        include/port/port_main.h
        src/port/os_yield_cpu.c
        include/port/os_yield_cpu.h)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(temu PRIVATE Threads::Threads)

if (PerformanceMode)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT supportLto OUTPUT output)
    if (supportLto)
        set_property(TARGET temu PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else ()
        message(WARNING "IPO is not supported: ${output}")
    endif ()
endif ()


add_custom_command(
    OUTPUT ${DOWNLOAD_DIR}/u-boot.bin
    COMMAND /bin/sh -c "if [ ! -f ${DOWNLOAD_DIR}/u-boot.bin ]; then wget ${URL} -O ${LOCAL_FILE_PATH} && unzip -o ${LOCAL_FILE_PATH} -d ${DOWNLOAD_DIR}; else echo 'Already extracted'; fi"
    COMMENT "Downloading ${FILE_NAME} using wget"
    VERBATIM
)

add_custom_target(download ALL
    DEPENDS ${DOWNLOAD_DIR}/u-boot.bin
    COMMENT "Download and extract file"
    COMMAND ${CMAKE_COMMAND} -E rm -f ${DOWNLOAD_DIR}/${FILE_NAME}
)

add_custom_target(temurun
    COMMAND ./temu --addr=0x81fa0000 --exec=../linux/fw_jump.bin "--with=0x80000000\#../linux/u-boot.bin" "--with=0x81ffd800\#../linux/u-boot.dtb" "--with=0x81000000\#../linux/uImage.gz"
    DEPENDS temu
    COMMENT "Running temu with specific arguments"
)

add_custom_target(run ALL
    DEPENDS temurun
)