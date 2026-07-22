cmake_minimum_required(VERSION 3.20)

project(huffman LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    include/huffman.h
    src/huffman.cpp
    test.c
)

target_include_directories(${PROJECT_NAME}
    PRIVATE src
    PUBLIC include
)
