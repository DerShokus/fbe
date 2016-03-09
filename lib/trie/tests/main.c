/*
 * main.c
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#define malloc test_malloc
#define calloc test_calloc
#define free test_free
#define realloc test_realloc

#include <trie.h>

static void trie_test_insertion(struct trie *trie, char *str, size_t n,
                                size_t o)
{
        void *old;
        bool ret =
            trie_insert(trie, (uint8_t *)str, strlen(str) + 1, (void *)n, &old);
        assert_true(ret); // successfull insertion
        assert_memory_equal(&old, &o, sizeof(old));
}

void test_trie_new(void **state)
{
        struct trie *x = trie_new();
        assert_non_null(x);
        *state = x;
}

void test_trie_delete(void **state)
{
        struct trie *x = *state;
        trie_delete(&x);
        assert_null(x);
        *state = x;
}

void test_trie_insert_empty(void **state)
{
        struct trie *x = *state;
        trie_test_insertion(x, "abc", 0xA, 0);
}

static void trie_test_create_delete(void **state)
{
        (void)state;

        struct trie *obj = trie_new();
        assert_non_null(obj);

        trie_delete(&obj);
        assert_null(obj);
}

static void trie_test_at(struct trie *trie, char *str, size_t must)
{
        void *data;
        bool ret = trie_at(trie, (uint8_t *)str, strlen(str) + 1, &data);
        assert_true(ret);
        assert_memory_equal(&data, &must, sizeof(data));
}

static void trie_test_insert(void **state)
{
        (void)state;

        static char word_1[] = "aaa";

        struct trie *obj = trie_new();
        assert_non_null(obj);

        trie_test_insertion(obj, word_1, 0xA, 0);
        trie_test_at(obj, word_1, 0xA);
}

static void trie_test_root_negative(void **state)
{
        (void)state;

        static char word_1[] = "aaa";
        static char word_2[] = "baa";

        struct trie *obj = trie_new();
        assert_non_null(obj);

        trie_test_insertion(obj, word_1, 0xA, 0);
        trie_test_insertion(obj, word_2, 0xB, 0);

        trie_test_at(obj, word_1, 0xA);
        trie_test_at(obj, word_2, 0xB);
}

static void trie_test_insert_midle_negative(void **state)
{
        (void)state;

        static char word_1[] = "aaa";
        static char word_2[] = "aba";

        struct trie *obj = trie_new();
        assert_non_null(obj);

        trie_test_insertion(obj, word_1, 0xA, 0);
        trie_test_insertion(obj, word_2, 0xB, 0);

        trie_test_at(obj, word_1, 0xA);
        trie_test_at(obj, word_2, 0xB);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
            cmocka_unit_test(trie_test_insert),
            cmocka_unit_test(trie_test_root_negative),
            cmocka_unit_test(trie_test_insert_midle_negative),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
