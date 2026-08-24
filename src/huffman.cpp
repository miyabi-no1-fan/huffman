#include "huffman.hpp"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <queue>
#include <stdexcept>
#include <vector>

#define CODE_LENGTH_LIMIT 16

using namespace Huffman;

struct Node {
    std::vector<uint8_t> symbols;
    uint32_t weight;

    bool operator<(const Node& other) const { return this->weight < other.weight; }
    bool operator>(const Node& other) const { return this->weight > other.weight; }
    bool operator>=(const Node& other) const { return this->weight >= other.weight; }
    bool operator<=(const Node& other) const { return this->weight <= other.weight; }
    bool operator==(const Node& other) const { return this->weight == other.weight; }
};

Dictionary Huffman::make_dictionary(const uint8_t* buf, const uint32_t len) {
    if (buf == nullptr || len == 0) {
        return {};
    }

    Dictionary dictionary{};

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> base_queue{};
    {
        std::vector<Node> literals(256, Node {});
        for (uint32_t i = 0; i < len; i++) {
            literals[buf[i]].symbols = { buf[i] };
            literals[buf[i]].weight += 1;
        }
        for (auto& literal : literals) {
            if (literal.weight > 0) {
                base_queue.push(literal);
            }
        }
    }

    if (base_queue.size() > (1ul << CODE_LENGTH_LIMIT)) {
        throw std::runtime_error("Number of unique symbols is larger than the number of all possible code can generate");
    }

    auto queue = base_queue;

    for (int level = CODE_LENGTH_LIMIT - 1; level > 0; level--) {
        auto next_queue = base_queue;

        while (queue.size() >= 2) {
            Node a = queue.top();
            queue.pop();
            Node b = queue.top();
            queue.pop();

            a.symbols.append_range(b.symbols);
            a.weight += b.weight;

            next_queue.push(std::move(a));
        }

        queue = std::move(next_queue);
    }

    int unique_symbols_count = base_queue.size();
    int select = 2 * unique_symbols_count - 2;

    for (int i = 0; i < select; i++) {
        Node a = queue.top();
        queue.pop();

        for (auto symbol : a.symbols) {
            dictionary[symbol].symbol = symbol;
            dictionary[symbol].length += 1;
        }
    }

    canonical_huffman_assignment(dictionary);

    return dictionary;
}

void Huffman::canonical_huffman_assignment(Dictionary& dictionary) {
    auto sorted_dict = dictionary;

    std::sort(sorted_dict.begin(), sorted_dict.end(), [](const auto& a, const auto& b) {
        return (a.length < b.length) || (a.length == b.length && a.symbol < b.symbol);
    });

    uint16_t code = 0;
    uint8_t len = 0;

    for (auto& symbol : sorted_dict) {
        if (symbol.length != 0) {
            if (symbol.length > len) {
                code <<= (symbol.length - len);
                len = symbol.length;
            }
            dictionary[symbol.symbol] = CodeWord {
                .code = code,
                .length = len,
                .symbol = symbol.symbol,
            };
            code += 1;
        }
    }
}

LookupTable Huffman::make_lookup_table(const Dictionary& dictionary) {
    LookupTable table;

    for (int i = 0; i < dictionary.size(); i++) {
        if (dictionary[i].length > 0) {
            uint8_t len = dictionary[i].length;
            uint16_t code = dictionary[i].code << (16 - len);

            for (uint32_t i = 0; i < (1 << (16 - len)); i++) {
                (*table)[code] = {
                    .length = len,
                    .symbol = static_cast<uint8_t>(i),
                };
                code += 1;
            }
        }
    }

    return table;
}
