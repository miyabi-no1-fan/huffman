cmake_minimum_required(VERSION 3.20)

project("huffman" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable("main" src/huffman.cpp main.cpp include/huffman.hpp)
target_include_directories("main" PUBLIC include)
