/*
 * heap.c
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "heap.h"

#include <assert.h>
#include <strings.h>
#include <stdio.h>

bool heap_init(struct heap *heap, uint8_t *data, size_t capasity)
{
        if (!heap || !data || !capasity)
                return false;

        heap->size = 0;
        heap->head = heap->data = data;

        heap->capasity = capasity;

        return true;
}

inline static void exchange(uint8_t *a, uint8_t *b)
{
        uint8_t c = *a;
        *a        = *b;
        *b        = c;
}

inline static size_t node_head(size_t index)
{
        if (!index)
                return 0;
        return (index - 1) / 2;
}

static void heap_fix_up(struct heap *heap)
{
        uint8_t *tree = heap->head;
        for (size_t i = heap->size - 1; i > 0; i = node_head(i)) {
                const size_t h = node_head(i);
                if (tree[i] < tree[h])
                        exchange(&tree[i], &tree[h]);
                else
                        break;
        }
}

bool heap_push(struct heap *heap, uint8_t item)
{
        if (!heap || heap->size == heap->capasity) {
                printf("size: %lu == %lu\n", heap->size, heap->capasity);
                return false;
        }

        // printf("size: %lu, capcasity: %lu, item: %u\n",
        //        heap->size, heap->capasity, item);
        assert(heap->size < heap->capasity);
        const size_t real_size = ((heap->head - heap->data) + heap->size);
        assert(real_size <= heap->capasity);
        if (real_size == heap->capasity) {
                // memmove(heap->data, heap->head, heap->size *
                // sizeof(*heap->head));
                for (size_t i = 0; i < heap->size; ++i) {
                        heap->data[i] = heap->head[i];
                }
                heap->head = heap->data;
        }
        /* set item as the last item of a heap and increment the size */
        heap->head[heap->size++] = item;
        /* finaly it needs a fixup an order */
        heap_fix_up(heap);

        return true;
}

//       [ 0                  [ 1
//    1          2            2   3
//  3   4     5     6        4 5 6 7 ]
// 7 8 9 10 11 12 13 14

static inline size_t node_left(size_t index) { return index * 2 + 1; }

static inline size_t node_right(size_t index) { return node_left(index) + 1; }

static void heap_fix_down(struct heap *heap)
{
        uint8_t *tree     = heap->head;
        const size_t size = heap->size;
        for (size_t i = 0; i < size; ++i) {
                const size_t left  = node_left(i);
                const size_t right = node_right(i);
                if (right < size && tree[left] > tree[right])
                        exchange(&tree[left], &tree[right]);
                if (left < size && tree[i] > tree[left])
                        exchange(&tree[i], &tree[left]);
        }
}

uint8_t heap_pop(struct heap *heap)
{
        if (!heap || !heap->head || !heap->size)
                return UINT8_MAX; /* TODO: fix it! */

        uint8_t res = *heap->head;
        ++heap->head;
        heap->size--;
        heap_fix_down(heap);

        return res;
}
