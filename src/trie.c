/*
 * trie.c
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "trie.h"

#include <stdlib.h>
#include <strings.h>
#include <assert.h>
#include <stdio.h>

struct trie_node {
        uint8_t atom; // symbol

        struct trie_node *negative; // negative node

        bool data_flag;
        union {
                struct trie_node *positive;
                void *data;
        };
};

struct trie {
        struct trie_node *root;
};

static inline struct trie_node *trie_node_new(uint8_t atom)
{
        struct trie_node *node = calloc(1, sizeof(struct trie_node));
        if (node)
                node->atom = atom;
        return node;
}

struct trie *trie_new(void)
{
        struct trie *trie = calloc(1, sizeof(struct trie));
        return trie;
}

void trie_delete(struct trie **trie)
{
        // TODO: удалить все остальные элементы дерева
        free(*trie);
        *trie = NULL;
}

struct find_res {
        const size_t sz;
        struct trie_node *last;
        struct trie_node *prev;
};

static inline struct find_res trie_find(struct trie_node *root,
                                        const uint8_t *str, const size_t size)
{
        if (root == NULL || str == NULL || size == 0) {
                struct find_res res = {.sz = 0, .last = NULL, .prev = NULL};
                return res;
        }

        size_t i               = 0;
        struct trie_node *node = root, *prev;

        for (i = 0; i <= (size - 1) && node;) {
                prev = node;
                if (node->atom == str[i]) {
                        // printf("find: + '%u' == '%u', %zu/%zu\n", node->atom,
                        // str[i], size, i);
                        node = node->positive;
                        ++i;
                } else {
                        // printf("find: - '%u' == '%u', %zu/%zu\n", node->atom,
                        // str[i], size, i);
                        node = node->negative;
                }
        }

        struct find_res res = {.sz = i, .last = node, .prev = prev};
        return res;
}

static inline struct trie_node *get_chain(const uint8_t *str, const size_t size,
                                          struct trie_node **last)
{
        if (str == NULL || size == 0)
                return NULL;

        struct trie_node *node = trie_node_new(str[0]), *res = node;
        if (node == NULL)
                return NULL;

        for (size_t i = 1; i < size; ++i) {
                node->positive = trie_node_new(str[i]);
                node           = node->positive;
                // TODO: in case of faile - remove all chain
        }

        if (last)
                *last = node;

        return res;
}

bool trie_insert(struct trie *root, const uint8_t *str, const size_t size,
                 void *data, void **old)
{
        // printf("insertion: %s\n", (char *)str);
        memset(old, 0, sizeof(*old));

        struct find_res found = trie_find(root->root, str, size);
        // printf("insertion found: %zu, %p, %p\n", found.sz, found.prev,
        // found.last);
        struct trie_node *last = NULL;
        if (found.sz == 0) {
                if (found.prev == NULL)
                        root->root = get_chain(str, size, &last);
                else
                        found.prev->negative = get_chain(str, size, &last);
                /*
                        for (struct trie_node *node = root->root;
                             node;
                             node = node->positive)
                            //printf("insertion: -> %u, %p\n", node->atom,
                   node);
                            */
        } else if (found.sz == size) {
                last = found.prev;
        } else {
                struct trie_node *tail =
                    get_chain(&str[found.sz], size - found.sz, &last);
                // printf("insertion: %u[%zu] == %u\n", str[found.sz], found.sz,
                // found.prev->atom);
                if (str[found.sz] == found.prev->atom) {
                        // printf("insertion: positive\n");
                        found.prev->positive = tail;
                } else {
                        // printf("insertion: negative\n");
                        found.prev->negative = tail;
                }
        }

        memcpy(&old, &last->data, sizeof(last->data));
        memcpy(&last->data, &data, sizeof(last->data));
        // printf("insertion: ->>> %u, %zu, %p\n", last->atom, last->data.sz,
        // last);

        return true;
}

bool trie_at(struct trie *root, const uint8_t *str, const size_t size,
             void **data)
{
        struct find_res found = trie_find(root->root, str, size);
        // printf("find: at %zu == %zu && %p, %p\n", found.sz, size, found.prev,
        // found.last);
        if (found.sz == size && found.prev) {
                memcpy(data, &found.prev->data, sizeof(*data));
                return true;
        }

        return false;
}

bool trie_remove(const uint8_t *str, const size_t size, void **data)
{
        return false;
}
