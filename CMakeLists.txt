cmake_minimum_required(VERSION 3.22)
project(temu)

set(CMAKE_CXX_STANDARD 17)

add_executable(temu src/main.cpp
        include/elf.h
        include/types.h
        src/elf_helper.cpp
        src/elf_helper.h
        src/machine.cpp
        src/machine.h
        src/peripherals.cpp
        src/peripherals.h
        src/peripherals/serial_8250.cpp
        src/peripherals/serial_8250.h
        src/irom.cpp
        src/irom.h
        src/iram.cpp
        src/iram.h)
