#include "huffman.h"

#include <assert.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

enum NodeType {
    LITERAL,
    NODE,
};

struct Node {
    enum NodeType type;
    uint8_t literal;
    size_t freq;
    struct Node* next;
    uint8_t bit;
};

static void reverse_quickSort(struct Node* src, const size_t len) noexcept(true);
static void update_rev_sorted_tree(std::vector<struct Node*>* rev_sorted_tree, struct Node* new_node) noexcept(true);

void destroy_huffman_dictionary(huffman_dictionary_t* dictionary) {
    free(dictionary);
}

/*
This generate a dictionaray follow standard Huffman algorithm

return a 256 enties Huffman lookup dictionary (for encoders)
return NULL on allocation failure

dictionary is allocated via calloc
*/
huffman_dictionary_t* make_huffman_dictionary(const void* src, const uint32_t len) {
    /*
    because len is a uint32_t the maximum sum of frequencies would be 2^32 - 1
    because our symbol size is 1 bytes, the maximum number of unique character is 256
    thus, our maxmimum code lengths from this algorithm is 44
    ```Python
    def fib(x: int) -> int:
        ...
    HARD_CAP = 2**32
    val = 0
    i = 0
    while val < HARD_CAP:
        val += fib(i) - 1
        i += 1
    print(i - 2 - 1)
    ```
    this is the code where we got the number 44,
    fib is the fibonacci
    */

    assert(len < UINT32_MAX);

    uint8_t* buf = (uint8_t*)src;

    huffman_dictionary_t* dictionary = (huffman_dictionary_t*)calloc(256, sizeof(*dictionary));  // 256 entry with code and length = 0
    if (dictionary == NULL) {
        return NULL;
    }
    huffman_symbol_t* dict = dictionary->dictionary;

    // no data, return empty dict
    if (buf == NULL || len == 0) {
        return dictionary;
    }

    std::vector<struct Node> tree;
    try {
        tree.reserve(256 * 2);  // a Huffman tree won't be bigger than this
        tree.resize(256);
    } catch (...) {
        free(dict);  // return NULL if allocation fail
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        tree[buf[i]].type = NodeType::LITERAL;
        tree[buf[i]].literal = buf[i];
        tree[buf[i]].freq += 1;  // count frequency
        tree[buf[i]].next = NULL;
        tree[buf[i]].bit = 0;
    }

    reverse_quickSort(tree.data(), tree.size());
    {
        assert(tree.size() == 256);

        size_t i = 0;
        for (; i < tree.size(); i++) {
            if (tree[i].freq == 0) {
                break;
            }
        }

        assert(i <= 256);
        try {
            tree.resize(i);  // remove Nodes with freq of 0
        } catch (...) {
            assert(false);
            /*
            i is <= 256
            this resize is a shrink,
            so there's no reallocation,
            => no exception
            */
        }
    }

    size_t number_of_unique_symbols = tree.size();

    // edge case: if exist only 1 unique character, it will get length 0 (infinite compression)
    // fix: assign code 0, length 1 for it
    if (number_of_unique_symbols == 1) {
        dict[tree[0].literal] = {
            .code = 0,
            .length = 1,
        };
        return dictionary;
    }

    /*
    Each Node is hardcoded with the next Node's addr, so we cannot sort the tree
    rev_sorted_tree are pointers point to the tree's addr in reverse sorted order
    */
    std::vector<struct Node*> rev_sorted_tree;
    try {
        rev_sorted_tree.resize(number_of_unique_symbols, NULL);
    } catch (...) {
        free(dict);
        return NULL;
    }

    // initialize rev_sorted_tree
    for (size_t i = 0; i < number_of_unique_symbols; i++) {
        rev_sorted_tree[i] = tree.data() + i;
    }

    //  this loop run for number_of_unique_symbols - 1 times
    while (rev_sorted_tree.size() >= 2) {
        // the last 2 element is the 2 Node with smallest freq
        struct Node* a = rev_sorted_tree[rev_sorted_tree.size() - 1];
        struct Node* b = rev_sorted_tree[rev_sorted_tree.size() - 2];
        rev_sorted_tree.resize(rev_sorted_tree.size() - 2);

        assert(tree.capacity() == 256 * 2);
        try {
            tree.push_back(Node{
                .type = NodeType::NODE,
                .literal = 0,  // ignored
                .freq = a->freq + b->freq,
                .next = NULL,
                .bit = 0,  // ignored
            });
        } catch (...) {
            assert(false);
            /*
            rev_sorted_tree size is 256 at max
            each loop pop 2 node then push 1 more
            => this loop will run 256 times at max
            => tree.push_back will be call 256 times at max,
            since tree is reserved with 256 * 2, no reallocation should happen
            */
        }

        a->bit = 0;
        b->bit = 1;
        a->next = &tree[tree.size() - 1];
        b->next = &tree[tree.size() - 1];

        update_rev_sorted_tree(&rev_sorted_tree, &tree[tree.size() - 1]);
    }

    for (size_t i = 0; i < tree.size(); i++) {
        if (tree[i].type == NodeType::LITERAL) {
            uint8_t literal = tree[i].literal;
            dict[literal].symbol = literal;
            struct Node* cur = &tree[i];
            // only root have cur->next == NULL
            while (cur->next != NULL) {
                dict[literal].code |= cur->bit << dict[literal].length;
                dict[literal].length += 1;
                cur = cur->next;
            }
        } else {
            // assume all LITERAL type are at the first indexes of the tree
            break;
        }
    }

    return dictionary;
}

static inline void update_rev_sorted_tree(std::vector<struct Node*>* rev_sorted_tree, struct Node* new_node) noexcept(true) {
    const size_t new_freq = new_node->freq;

    // binary search
    size_t lo = 0;
    size_t hi = (*rev_sorted_tree).size();
    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        // the new Nodes should be bellow the existed Node that have the same frequency
        // this have to be a '<=' sign
        if (new_freq <= (*rev_sorted_tree)[mid]->freq)
            lo = mid + 1;
        else
            hi = mid;
    }

    try {
        (*rev_sorted_tree).insert((*rev_sorted_tree).begin() + lo, new_node);
    } catch (...) {
        assert(false);
        // because rev_sorted_tree size is reducing by 1 each loop, there shouldn't be any realloc, thus, no exception
    }
}

static inline void reverse_quickSort(struct Node* src, const size_t len) noexcept(true) {
    if (len <= 1) {
        return;
    }

    size_t j = 0;
    size_t i = 0;
    struct Node pivot = src[len - 1];

    while (i < len - 1) {
        if (src[i].freq > pivot.freq) {
            struct Node temp = src[i];
            src[i] = src[j];
            src[j] = temp;
            j += 1;
        }
        i += 1;
    }
    struct Node temp = src[i];
    src[i] = src[j];
    src[j] = temp;

    reverse_quickSort(src, j);
    reverse_quickSort(src + j + 1, len - (j + 1));
}
