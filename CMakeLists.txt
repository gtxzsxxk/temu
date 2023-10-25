cmake_minimum_required(VERSION 3.22)
project(temu)

set(CMAKE_CXX_STANDARD 17)

add_executable(temu src/main.cpp
        include/elf.h
        include/types.h)
