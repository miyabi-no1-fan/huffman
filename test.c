#include <stdio.h>
#include <stdlib.h>

#include "huffman.h"

int main() {
    const char msg[] = "Hello World Hello World";
    huffman_dictionary_t* dict = make_huffman_dictionary(msg, sizeof(msg));
    huffman_symbol_t* dictionary = dict->dictionary;

    for (size_t i = 0; i < 256; i++) {
        if (dictionary[i].length != 0)
            printf("%c -> huffman_table_t { code: %lu, length: %u }\n", (const char)i, dictionary[i].code, dictionary[i].length);
    }

    free(dict);
    return 0;
}
