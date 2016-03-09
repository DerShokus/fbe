/*
 * trie.h
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TRIE_H
#define TRIE_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

struct trie;

/*
 * TODO:
 */
struct trie *trie_new(void);
/*
 * TODO:
 */
void trie_delete(struct trie **trie);

bool trie_insert(struct trie *root, const uint8_t *str, const size_t size,
                 void *data, void **old);

bool trie_at(struct trie *root, const uint8_t *str, const size_t size,
             void **data);

bool trie_remove(const uint8_t *str, const size_t size, void **data);

#endif /* !TRIE_H */
