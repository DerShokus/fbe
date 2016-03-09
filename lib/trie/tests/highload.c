/*
 * highload.c
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <trie.h>
#include <stdio.h>
#include <ctype.h>

int main(void)
{
        struct trie *obj = trie_new();
        void *data, *old;
        char *str;
        size_t str_cap;
        ssize_t str_len;
        FILE *input = fopen("/usr/share/dict/words", "r");
        char first  = '\0';

        for (size_t i = 0;; ++i) {
                str_len = getline(&str, &str_cap, input);
                if (str_len == -1) {
                        // printf("Insert end. Total: %zu\n", i);
                        break;
                }
                if (toupper(first) != toupper(str[0])) {
                        first = str[0];
                        // printf("Insert %c...\n", first);
                }
                str[str_len - 1] = '\0';
                data = (void *)i;
                if (!trie_insert(obj, (uint8_t *)str, str_len, data, &old)) {
                        printf("ERROR: trie insert '%s' failed\n", str);
                        return 1;
                }
        }

        fseek(input, 0L, SEEK_SET);

        for (size_t i = 0;; ++i) {
                str_len = getline(&str, &str_cap, input);

                if (str_len == -1) {
                        printf("Check end.  Total: %zu\n", i);
                        break;
                }
                str[str_len - 1] = '\0';
                if (!trie_at(obj, (uint8_t *)str, str_len, &old)) {
                        printf("Check ERROR: '%s' %zu, failed\n", str, str_len);
                        return 1;
                }
                if (old != (void *)i) {
                        printf("Check failed: '%s' data: %zu != %zu\n", str,
                               (size_t)old, i);
                        return 1;
                }
        }

        return 0;
}
