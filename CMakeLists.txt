cmake_minimum_required(VERSION 3.22)
project(temu C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")

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
        include/perf.h)
