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

/*
 * Trie object. All fields are hidden
 */
struct trie;

/*
 * Trie node.
 */
struct trie_node;

/*
 * Allocator for trie object and all trie's nodes.
 */
typedef void *(*trie_allocator_t)(size_t);

/*
 * Delete memory which allocated by trie_allocator_t)
 */
typedef void (*trie_deallocator_t)(void *);

/*
 * Create new trie object.
 * Returns trie object or null if operation failed.
 * If allocator is NULL - trie uses malloc.
 * If deallocator is NULL - trie uses free.
 */
struct trie *trie_new(trie_allocator_t allocator,
                      trie_deallocator_t deallocator);
/*
 * Delete an object and all data which contained there.
 * Pointer to an object sets to NULL.
 */
void trie_delete(struct trie **trie);

/*
 * Insert new data to the tree.
 * Previous data associated with the key will returned by old parameter and will
 * be replaced by data parameter.
 *
 * Returns true if operation completed successfull.
 */
bool trie_insert(struct trie *root, const uint8_t *key, const size_t key_size,
                 void *data, void **old);

/*
 * Get value associated with the key.
 * Value returns by data parameter.
 *
 * Returns true if key contains in the tree and the operation completed
 * successfull.
 */
bool trie_at(struct trie *root, const uint8_t *key, const size_t key_size,
             void **data);

/*
 * Remove subtree for the key.
 * The old value returns with data parameter.
 *
 * Returns true if the operation completed successfull.
 */
bool trie_remove(struct trie *obj, const uint8_t *key, const size_t key_size,
                 void **data);

/*
 *
 */
struct trie_node *trie_begin(struct trie *trie);

/*
 *
 */
struct trie_node *trie_next(struct trie_node *node);

/*
 *
 */
struct trie_node *trie_next_delete(struct trie *obj, struct trie_node *node);

/*
 *
 */
bool trie_data(struct trie_node *node, void **data);

#endif /* !TRIE_H */
