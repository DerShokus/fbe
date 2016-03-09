/*
 * eval.c
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include <fbe.h>
#include "../src/opcode.h"
#include "../src/command.h"
#include "../src/lexer.h"
#include "../src/ast.h"

#include "test_case.h"

/*
 * Test cases
 */
TEST_CASE_IMP(and, "A && B;")
return args[0] && args[1];
TEST_CASE_END(and, 2)

TEST_CASE_IMP(or, "A || B;")
return args[0] || args[1];
TEST_CASE_END(or, 2)

TEST_CASE_IMP(and_or, "A && B || C;")
return args[0] && args[1] || args[2];
TEST_CASE_END(and_or, 3)

TEST_CASE_IMP(or_and, "A || B && C;")
return args[0] || args[1] && args[2];
TEST_CASE_END(or_and, 3)

TEST_CASE_IMP(1, "A || B && C || D && E || F && G || H && I;")
return args[0] || args[1] && args[2] || args[3] && args[4] ||
       args[5] && args[6] || args[7] && args[8];
TEST_CASE_END(1, 9)

TEST_CASE_IMP(2, "((((((((A && B) || C) && D) || E) && F) || G) && H) || I);")
return ((((((((args[0] && args[1]) || args[2]) && args[3]) || args[4]) &&
           args[5]) ||
          args[6]) &&
         args[7]) ||
        args[8]);
TEST_CASE_END(2, 9)

TEST_CASE_IMP(3, "((A || B) && (C || D)) || ((E || F) && (G || H));")
return ((args[0] || args[1]) && (args[2] || args[3])) ||
       ((args[4] || args[5]) && (args[6] || args[7]));
TEST_CASE_END(3, 8)

TEST_CASE_IMP(4, "(A || (B && (C || (D && (E || (F && (G || (H && I))))))));")
return (args[0] ||
        (args[1] &&
         (args[2] ||
          (args[3] &&
           (args[4] || (args[5] && (args[6] || (args[7] && args[8]))))))));
TEST_CASE_END(4, 9)

bool increment(bool *num, size_t size)
{
        bool counter = false;
        for (size_t i = 0; i < size; ++i) {
                if (num[i]) {
                        num[i]  = false;
                        counter = true;
                } else {
                        counter = false;
                        num[i]  = true;
                        break;
                }
        }
        return !counter;
}

struct combinations {
        bool **args;
        size_t args_count;
        size_t size;
};

struct combinations combinations(size_t args_count)
{
        const size_t cmb_count = 1 << args_count;
        const size_t args_size = sizeof(bool) * args_count;
        bool **args = calloc(cmb_count, sizeof(bool *));
        for (size_t i = 0; i < cmb_count; ++i) {
                args[i] = malloc(args_size);
        }
        memset(args[0], 0, args_size);

        for (size_t i = 1; i < cmb_count; ++i) {
                memcpy(args[i], args[i - 1], args_size);
                increment(args[i], args_count);
        }

        struct combinations cmb = {
            .args = args, .args_count = args_count, .size = cmb_count};
        return cmb;
}

int main(void)
{
        struct test_case *tests[] = {
            get_test_case_and(), get_test_case_or(), get_test_case_1(),
            get_test_case_2(),   get_test_case_3(),  get_test_case_4(),
        };

        for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
                struct combinations cmb = combinations(tests[i]->args_count);
                bool *fbe_args          = calloc(tests[i]->args_count, sizeof(bool));
                struct fbe fbe;
                if (fbe_compile(&fbe, tests[i]->source) != FBE_OK) {
                        printf("COMPILE '%s' FAILED\n", tests[i]->source);
                        break;
                }
                for (size_t j = 0; j < cmb.size; ++j) {
                        const bool c_result = tests[i]->func(cmb.args[j]);

                        for (size_t k = 0; k < cmb.args_count; ++k) {
                                char arg[2]          = {'\0'};
                                arg[0]               = 'A' + k;
                                const enum fbe_rc rc = fbe_arg_set(
                                    &fbe, fbe_args, tests[i]->args_count, arg,
                                    cmb.args[j][k]);
                                // printf("rc: %d, var: %s\n", rc, arg);
                                assert(rc == FBE_OK);
                        }
                        const struct fbe_res rc =
                            fbe_eval(&fbe, fbe_args, cmb.args_count);
                        printf("rc: %d\n", rc.code);
                        assert(rc.code == FBE_OK);

                        if (c_result != rc.value) {
                                printf("TEST FAILED: code: '%s'\nargs: ",
                                       tests[i]->source);
                                printf("FBE PARAMS: ");
                                for (size_t k = 0; k < cmb.args_count; ++k) {
                                        char arg[2]              = {'\0'};
                                        arg[0]                   = 'A' + k;
                                        const struct fbe_res val = fbe_arg_get(
                                            &fbe, fbe_args,
                                            tests[i]->args_count, arg);
                                        printf("%c: %s, ", arg[0],
                                               val.value ? "true" : "false");
                                        assert(rc.code == FBE_OK);
                                }
                                printf("\n");
                                for (size_t k = 0; k < cmb.args_count; ++k)
                                        printf("%s[%zu], ",
                                               cmb.args[j][k] ? "true"
                                                              : "false",
                                               k);
                                printf("\n");
                                printf("fbe: %s, c: %s\n",
                                       rc.value ? "true" : "false",
                                       c_result ? "true" : "false");
                                assert(false);
                        } else {
                                // printf("TEST OK: code: '%s'\nargs: ",
                                // tests[i]->source);
                                // for (size_t k = 0; k < cmb.args_count; ++k)
                                //    printf("%s[%zu], ", cmb.args[j][k] ?
                                //    "true" : "false", k);
                                // printf("\n");
                                // printf("fbe: %s, c: %s\n", fbe_result ?
                                // "true" : "false",
                                //       c_result ? "true" : "false");
                        }
                }
        }
        return 0;
}
