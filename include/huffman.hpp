// PACKAGE-MERGE ALGORITHM (LENGTH-LIMITED HUFFMAN)

#pragma once
#include <array>
#include <cstdint>
#include <format>
#include <memory>
#include <ostream>

namespace Huffman {

struct CodeWord {
    uint16_t code;   // encoded symbol's value
    uint8_t length;  // encoded symbol's length
    uint8_t symbol;
};

// a LookupTable entry
struct Symbol {
    uint8_t length;  // encoded symbol's length
    uint8_t symbol;
};

// For encoder
using Dictionary = std::array<CodeWord, 256>;

// For decoder
using LookupTable = std::unique_ptr<std::array<Symbol, 65536>>;

/* For encoder.
Generate a dictionary,
where,
    `code_for_character_s = dictionary[s]`.
assume 0 <= s < 256. */
Dictionary make_dictionary(const uint8_t* buf, const uint32_t len);

/* For decoder.
Generate a lookup table from dictionary.
where,
lookup_table[code] is the dedicated symbol for that code with its code_length.
assume 0 <= code < 65536. */
LookupTable make_lookup_table(const Dictionary& dictionary);

// asign valid Huffman code for a dictionary that has length and symbol only
void canonical_huffman_assignment(Dictionary& dictionary);

static std::ostream& operator<<(std::ostream& os, const CodeWord& code) {
    return os << "CodeWord { symbol: " << code.symbol << ", code: " << std::format("{:0{}b}", code.code, code.length) << ", length: " << (int)code.length << " }";
}

static std::ostream& operator<<(std::ostream& os, const Symbol& sym) {
    return os << "Symbol { symbol: " << sym.symbol << ", code_length: " << (int)sym.length << " }";
}

static std::ostream& operator<<(std::ostream& os, const Dictionary& dict) {
    for (auto& code : dict) {
        if (code.length > 0) {
            os << code << "\n";
        }
    }
    return os;
}

static std::ostream& operator<<(std::ostream& os, const LookupTable& table) {
    for (uint32_t i = 0; i < (*table).size(); i++) {
        if ((*table)[i].length > 0) {
            os << CodeWord {
                .code = static_cast<uint16_t>(i),
                .length = (*table)[i].length,
                .symbol = (*table)[i].symbol,
            };
        }
    }
    return os;
}

}  // namespace Huffman