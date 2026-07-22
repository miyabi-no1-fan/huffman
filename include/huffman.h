#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t code;
    uint8_t length;  // length = 0 means no code for this symbol
    uint8_t symbol;
} huffman_symbol_t;

typedef struct {
    huffman_symbol_t dictionary[256];
} huffman_dictionary_t;

huffman_dictionary_t* make_huffman_dictionary(const void* buf, const uint32_t len);

void destroy_huffman_dictionary(huffman_dictionary_t* dictionary);

#ifdef __cplusplus
}
#endif