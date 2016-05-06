/*
 * args.c
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <fbe.h>
#include <assert.h>

struct arg {
        bool *val;
        char *name;
};

int main(void)
{
        const char source[] =
            "a0 && a1 || a2 && a3 || a4 && a5 || a6 && a7 || a8 && a9;";
        bool args_buf[10]   = {false};
        struct arg args[10] = {
            {.val = &args_buf[0], .name = "a0"},
            {.val = &args_buf[1], .name = "a1"},
            {.val = &args_buf[2], .name = "a2"},
            {.val = &args_buf[3], .name = "a3"},
            {.val = &args_buf[4], .name = "a4"},
            {.val = &args_buf[5], .name = "a5"},
            {.val = &args_buf[6], .name = "a6"},
            {.val = &args_buf[7], .name = "a7"},
            {.val = &args_buf[8], .name = "a8"},
            {.val = &args_buf[9], .name = "a9"},
        };

        struct fbe fbe;
        enum fbe_rc rc = fbe_compile(&fbe, source);
        assert(rc == FBE_OK);
        for (size_t i = 0; i < sizeof(args) / sizeof(args[0]); ++i) {
                // printf("1: changing %s to %s...\n", args[i].name,
                // !*args[i].val ? "true" : "false");
                for (size_t j = 0; j < 10; ++j) {
                        assert(!args_buf[j]);
                }
                rc = fbe_arg_set(&fbe, args_buf, 10, args[i].name,
                                 !*args[i].val);
                assert(rc == FBE_OK);
                int counter = 0;
                for (size_t j = 0; j < 10; ++j) {
                        if (args_buf[j])
                                counter++;
                }
                assert(counter == 1);
                counter = 0;
                rc      = fbe_arg_set(&fbe, args_buf, 10, args[i].name,
                                 !*args[i].val);
                assert(rc == FBE_OK);
                for (size_t j = 0; j < 10; ++j) {
                        if (args_buf[j])
                                counter++;
                }
                assert(counter == 0);
        }

        return 0;
}
