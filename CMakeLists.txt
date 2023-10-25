cmake_minimum_required(VERSION 3.22)
project(temu)

set(CMAKE_CXX_STANDARD 17)

add_executable(temu src/main.cpp
        include/elf.h
        include/types.h
        src/elf_helper.cpp
        src/elf_helper.h
        src/cpu-wip-0001/machine.cpp
        src/cpu-wip-0001/machine.h
        src/cpu-wip-0001/peripherals.cpp
        src/cpu-wip-0001/peripherals.h
        peripherals/serial_8250.cpp
        peripherals/serial_8250.h
        src/cpu-wip-0001/irom.cpp
        src/cpu-wip-0001/irom.h
        src/cpu-wip-0001/iram.cpp
        src/cpu-wip-0001/iram.h
        include/cpu.h
        include/ram.h
        include/rom.h
        src/cpu.cpp)
