
#ifndef FBE_H
#define FBE_H

#include <stddef.h>
#include <stdbool.h>

struct fbe {
        void *code;
        void *context;
};

enum fbe_rc {
        FBE_OK                 = 0,
        FBE_FAIL               = 1,
        FBE_VAR_NOT_FOUND      = 2,
        FBE_ARGS_OUT_OF_BOUNDS = 3,
        FBE_INVALID_ARGUMENTS  = 4,
        FBE_COMPILATION_ERROR  = 5,
        FBE_RUN_TIME_FAIL      = 6,
};

struct fbe_res {
        enum fbe_rc code;
        bool value;
};

/*
 * Compile sources of program to bytecode and associate it with fbe-object.
 */
enum fbe_rc fbe_compile(struct fbe *fbe, const char *sourcecode);

/*
 * Evaluate program associated with the fbe-object.
 */
struct fbe_res fbe_eval(const struct fbe *fbe, const bool args[],
                        size_t args_total);

/*
 * Set argument `var` to `args` array of argument by special information in
 * the fbe object.
 */
enum fbe_rc fbe_arg_set(const struct fbe *fbe, bool args[], size_t args_total,
                        const char *var, bool val);

/*
 * Get argumnet's value
 */
struct fbe_res fbe_arg_get(const struct fbe *fbe, bool args[],
                           size_t args_total, const char *var);

#endif /* !FBE_H */
