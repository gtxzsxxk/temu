cmake_minimum_required(VERSION 3.22)
project(temu C)

set(CMAKE_C_STANDARD 11)

set(PerformanceMode OFF)
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
        include/port/system_timer.h)

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
