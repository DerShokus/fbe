/*
 * heap.h
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>
#include <stdbool.h>

struct heap {
        uint8_t *head;
        size_t size;

        uint8_t *data;
        size_t capasity;

        /* TODO: Add ordering */
};

bool heap_init(struct heap *heap, uint8_t *data, size_t capasity);

bool heap_push(struct heap *heap, uint8_t item);

uint8_t heap_pop(struct heap *heap);

#endif /* !HEAP_H */
