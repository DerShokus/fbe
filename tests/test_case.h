/*
 * test_case.h
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TEST_CASE_H
#define TEST_CASE_H

#include <stdbool.h>
#include <stdlib.h>

#define TEST_CASE_IMP(__name__, __source__)                                    \
        static char test_expr_##__name__[] = __source__;                       \
        bool test_func_##__name__(const bool args[])                           \
        {

#define TEST_CASE_END(__name__, __args_count__)                                \
        }                                                                      \
        struct test_case *get_test_case_##__name__(void)                       \
        {                                                                      \
                struct test_case *test = malloc(sizeof(struct test_case));     \
                test->func             = test_func_##__name__;                 \
                test->args_count       = __args_count__;                       \
                test->source           = test_expr_##__name__;                 \
                return test;                                                   \
        }

struct test_case {
        bool (*func)(const bool args[]);
        size_t args_count;
        char *source;
};

#endif /* !TEST_CASE_H */
