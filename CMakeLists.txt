cmake_minimum_required(VERSION 3.22)
project(temu C)

set(CMAKE_C_STANDARD 11)

set(PerformanceMode OFF)
set(PerformanceMonitor ON)
if (PerformanceMode)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g")
    message(STATUS "Compiling with performance mode -- -O2")
else ()
    message(STATUS "Compiling without performance")
endif ()
if (NOT PerformanceMonitor)
    add_definitions(-DPERF_MUTE_ALL)
endif ()

include_directories(include)

add_executable(temu src/main.c
        src/mem.c
        src/machine.c
        src/decode.c
        src/zicsr.c
        include/zicsr.h
        src/trap.c
        include/trap.h
        include/uart8250.h
        src/peripherals/uart8250.c
        src/vm.c
        include/vm.h
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
        include/cache.h)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(temu PRIVATE Threads::Threads)
