cmake_minimum_required(VERSION 3.22)
project(temu C)

set(CMAKE_C_STANDARD 11)

include_directories(include)
include_directories(lib)
include_directories(src)
include_directories(src/*)

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
        include/port/lock.h)
