#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "huffman.hpp"

int main() {
    const char msg[] = "ABBCCCDDDDEEEEEFFFFFFGGGGGGGHHHHHHHH";
    auto dict = Huffman::make_dictionary((uint8_t*)msg, sizeof(msg) - 1);
    std::cout << dict;
    auto table = Huffman::make_lookup_table(dict);
    
    for (unsigned i = 0; i < dict.size(); i++) {
        if (dict[i].length > 0) {
            auto code = dict[i].code;
            auto len = dict[i].length;
            auto symbol = dict[i].symbol;

            if (symbol != (*table)[code << (16 - len)].symbol ||
                len != (*table)[code << (16 - len)].length)
                std::abort();
        }
    }

    return 0;
}
