#include <cstdint>
#include <iostream>

#include "huffman.hpp"

int main() {
    const char msg[] = "ABBCCCDDDDEEEEEFFFFFFGGGGGGGHHHHHHHH";
    auto dict = Huffman::make_dictionary((uint8_t*)msg, sizeof(msg) - 1);
    std::cout << dict;
    return 0;
}
