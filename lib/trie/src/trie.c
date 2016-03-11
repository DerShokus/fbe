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

static struct trie_node *end_of_negative(struct trie_node *node)
{
        while (node->negative) {
                node = node->negative;
        }
        return node;
}

static struct trie_node *delete_positive(struct trie_node *root,
                                         struct trie_node **end_global)
{
        struct trie_node *end = *end_global;
        // delete all positive branches
        struct trie_node *node = root->positive;
        while (node) {
                // skip the node
                if (node->data_flag) {
                        root->positive = NULL;
                } else {
                        root->positive = node->positive;
                }
                // add negative branch to the end list
                if (node->negative) {
                        end->negative  = node->negative;
                        end            = end_of_negative(end);
                        node->negative = NULL;
                }
                // delete node and get next
                free(node);
                node = root->positive;
        }
        // delete the root node
        node = root->negative;
        free(root);
        // update the end list
        *end_global = end;

        return node;
}

void trie_delete(struct trie **trie)
{
        if (!trie || !(*trie))
                return;
        if ((*trie)->root) {
                struct trie_node *root = (*trie)->root;
                struct trie_node *end  = end_of_negative(root);

                while (root) {
                        root = delete_positive(root, &end);
                }
        }
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
                        node = node->positive;
                        ++i;
                } else {
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
        memset(old, 0, sizeof(*old));

        struct find_res found  = trie_find(root->root, str, size);
        struct trie_node *last = NULL;
        if (found.sz == 0) {
                if (found.prev == NULL)
                        root->root = get_chain(str, size, &last);
                else
                        found.prev->negative = get_chain(str, size, &last);

        } else if (found.sz == size) {
                last = found.prev;
        } else {
                struct trie_node *tail =
                    get_chain(&str[found.sz], size - found.sz, &last);
                if (str[found.sz] == found.prev->atom) {
                        found.prev->positive = tail;
                } else {
                        found.prev->negative = tail;
                }
        }

        memcpy(&old, &last->data, sizeof(last->data));
        memcpy(&last->data, &data, sizeof(last->data));
        last->data_flag = true;

        return true;
}

bool trie_at(struct trie *root, const uint8_t *str, const size_t size,
             void **data)
{
        struct find_res found = trie_find(root->root, str, size);
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
