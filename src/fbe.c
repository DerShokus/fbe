
#include <fbe.h>
#include <trie.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "opcode.h"
#include "command.h"
#include "heap.h"
#include "lexer.h"

union var_index {
        void *p;
        size_t zu;
};

/*
 *
 */
struct fbe_context {
        // size_t registers_count;
        bool *registers;
        struct trie *arguments;
        size_t arguments_count;
};

static struct fbe_context *fbe_context_new(void)
{
        struct fbe_context *context = calloc(1, sizeof(struct fbe_context));
        if (context != NULL) {
                context->arguments = trie_new();
                if (context->arguments == NULL) {
                        free(context);
                        context = NULL;
                }
        }
        return context;
}

static void fbe_context_delete(struct fbe_context *context)
{
        if (context) {
                free(context->registers);
                trie_delete(&context->arguments);
        }
}

struct fbe_arg_idx {
        bool found;
        union {
                void *p;
                size_t id;
        };
};

inline static struct fbe_arg_idx fbe_arg_at(const struct fbe_context *context,
                                            const char *var)
{
        const uint8_t *key   = (uint8_t *)var;
        const size_t key_len = strlen(var) + 1;

        struct fbe_arg_idx idx;
        idx.found = trie_at(context->arguments, key, key_len, &idx.p);

        return idx;
}

enum fbe_rc fbe_arg_set(const struct fbe *fbe, bool args[], size_t args_total,
                        const char *var, bool val)
{
        if (!fbe || !args | !args_total || !var || var[0] == '\0') {
                return FBE_INVALID_ARGUMENTS;
        }

        const struct fbe_arg_idx idx = fbe_arg_at(fbe->context, var);
        if (!idx.found) {
                return FBE_VAR_NOT_FOUND;
        }

        if (idx.id >= args_total) {
                return FBE_ARGS_OUT_OF_BOUNDS;
        }
        args[idx.id] = val;

        return FBE_OK;
}

struct fbe_res fbe_arg_get(const struct fbe *fbe, bool args[],
                           size_t args_total, const char *var)
{
        struct fbe_res res;
        if (!fbe || !args || args_total == 0 || !var || var[0] == '\0') {
                res.code = FBE_INVALID_ARGUMENTS;
                return res;
        }

        const struct fbe_arg_idx idx = fbe_arg_at(fbe->context, var);
        if (!idx.found) {
                res.code = FBE_VAR_NOT_FOUND;
                return res;
        }

        if (idx.id >= args_total) {
                res.code = FBE_ARGS_OUT_OF_BOUNDS;
                return res;
        }
        res.value = args[idx.id];
        res.code  = FBE_OK;

        return res;
}

static struct fbe_arg_idx fbe_arg_get_or_insert(struct fbe_context *context,
                                                const char *name)
{
        struct fbe_arg_idx idx = fbe_arg_at(context, name);
        if (idx.found) {
                return idx;
        }
        union var_index old;
        // not found - add it
        idx.id = context->arguments_count;
        if (trie_insert(context->arguments, (const uint8_t *)name,
                        strlen(name) + 1, idx.p, &old.p)) {
                context->arguments_count++;
                idx.found = true;
                return idx;
        }

        idx.found = false;
        return idx;
}

static struct argument fbe_node2arg(struct fbe_context *ctx,
                                    struct ast_node *node)
{
        struct argument arg;
        if (node->opcode == TOKEN_VARIABLE) {
                arg.type = ARGUMENT_TYPE_DEFAULT;
                const struct fbe_arg_idx idx =
                    fbe_arg_get_or_insert(ctx, node->token);
                arg.external_id = idx.id;
        } else {
                assert(node->data != NULL);
                arg.type        = ARGUMENT_TYPE_REGISTER;
                arg.register_id = command_register_get(node->data);
        }
        return arg;
}

static struct command *fbe_node2cmd(struct fbe_context *ctx,
                                    struct ast_node *node, uint8_t reg_result,
                                    opcode_t *code)
{
        assert(node->opcode != TOKEN_VARIABLE);

        struct command *cmd = (struct command *)code;
        command_init(cmd);
        enum opcode opcode;
        switch (node->opcode) {
        case TOKEN_OR: opcode  = OPCODE_LOGICAL_OR; break;
        case TOKEN_AND: opcode = OPCODE_LOGICAL_AND; break;
        default: return NULL;
        }
        command_opcode_set(cmd, opcode);
        command_register_set(cmd, reg_result);

        command_arg_add(cmd, fbe_node2arg(ctx, node->left));
        command_arg_add(cmd, fbe_node2arg(ctx, node->right));

        node->data = cmd;
        return cmd;
}

inline static bool fbe_is_compileble(struct ast_node *node)
{
        if (!node || !node->left || !node->right) {
                return false;
        }
        if (node->opcode == TOKEN_VARIABLE) {
                return false;
        }

        if (node->left->opcode != TOKEN_VARIABLE && !node->left->data) {
                return false;
        }
        if (node->right->opcode != TOKEN_VARIABLE && !node->right->data) {
                return false;
        }

        return true;
}

inline static bool fbe_is_right_var(const struct ast_node *node)
{
        return node->right && node->right->opcode == TOKEN_VARIABLE;
}

inline static bool fbe_is_left_var(const struct ast_node *node)
{
        return node->left && node->left->opcode == TOKEN_VARIABLE;
}

struct node_stack {
        struct ast_node *node;
        struct node_stack *prev;
};

inline static bool stack_push(struct node_stack **top, struct ast_node *node)
{
        struct node_stack *item = malloc(sizeof(struct node_stack));
        if (item) {
                item->prev = *top;
                item->node = node;
                *top       = item;
                return true;
        }
        return false;
}

inline static struct ast_node *stack_pop(struct node_stack **top)
{
        struct ast_node *node  = (*top)->node;
        struct node_stack *ret = *top;
        *top = ret->prev;
        free(ret);
        return node;
}

inline static struct ast_node *stack_pick(struct node_stack *top)
{
        return top->node;
}

static void *fbe_ast2code(struct fbe_context *context, struct ast_node *root)
{
        // TODO: make it unlimit
        opcode_t *result    = calloc(1024, sizeof(opcode_t));
        struct command *cmd = (struct command *)result;
        uint8_t rdata[UINT8_MAX];
        struct heap rheap;
        heap_init(&rheap, rdata, sizeof(rdata) / sizeof(*rdata));
        for (size_t i = 0; i < 256; ++i) {
                heap_push(&rheap, (uint8_t)i);
        }

        struct node_stack *top = NULL;

        stack_push(&top, root);
        while (top) {
                struct ast_node *node = stack_pick(top);

                if (fbe_is_compileble(node)) {
                        stack_pop(&top);
                        cmd = fbe_node2cmd(context, node, heap_pop(&rheap),
                                           (opcode_t *)cmd);
                        // return used register numbers
                        for (uint8_t i = 0; i < command_args_count(cmd); ++i) {
                                struct argument *arg = command_arg_at(cmd, i);
                                if (arg &&
                                    arg->type == ARGUMENT_TYPE_REGISTER) {
                                        heap_push(&rheap, arg->register_id);
                                }
                        }
                        cmd = command_next(cmd);
                        command_init(cmd);
                } else {
                        node->data = NULL;
                }
                // push to stack
                if (!fbe_is_right_var(node) && !node->right->data) {
                        stack_push(&top, node->right);
                }
                if (!fbe_is_left_var(node) && !node->left->data) {
                        stack_push(&top, node->left);
                }
        }
        cmd         = command_next(cmd);
        cmd->opcode = OPCODE_END;

        return result;
}

inline static bool fbe_valid(const struct fbe *fbe)
{
        if (fbe && fbe->code && fbe->context) {
                return true;
        }
        return false;
}

struct fbe_res fbe_eval(const struct fbe *fbe, const bool args[],
                        size_t args_total)
{
        struct fbe_res rc;
        if (!fbe_valid(fbe) || args == NULL || args_total == 0) {
                rc.code = FBE_INVALID_ARGUMENTS;
                return rc;
        }

        struct command *cmd = (struct command *)fbe->code;
        bool *regs          = calloc((1 + register_size(cmd)), sizeof(bool));

        bool res;
        for (opcode_t opcode = command_opcode_get(cmd); opcode != OPCODE_END;
             opcode                       = command_opcode_get(cmd)) {
                const uint8_t register_id = command_register_get(cmd);
                switch (opcode) {
                case OPCODE_LOGICAL_OR: regs[register_id]  = false; break;
                case OPCODE_LOGICAL_AND: regs[register_id] = true; break;
                case OPCODE_END:
                        // TODO: Unexpected result
                        rc.value = res;
                        rc.code  = FBE_OK;
                        goto finish;
                default: assert(false);
                }

                const uint8_t cmd_count = command_args_count(cmd);
                for (uint8_t j = 0; j < cmd_count; ++j) {
                        const struct argument *arg = command_arg_at(cmd, j);
                        switch (arg->type) {
                        case ARGUMENT_TYPE_DEFAULT:
                                if (arg->external_id < args_total) {
                                        res = args[arg->external_id];
                                } else {
                                        rc.code = FBE_RUN_TIME_FAIL;
                                        goto finish;
                                }
                                break;
                        case ARGUMENT_TYPE_REGISTER:
                                res = regs[arg->register_id];
                                break;
                        default: rc.code = FBE_RUN_TIME_FAIL; goto finish;
                        }
                        if (res != regs[register_id]) {
                                regs[register_id] = res;
                                break;
                        }
                }
                cmd = command_next(cmd);
        }

        rc.value = res;
        rc.code  = FBE_OK;
finish:
        free(regs);

        return rc;
}

inline static void fbe_clean(struct fbe *fbe)
{
        if (fbe->context) {
                fbe_context_delete(fbe->context);
                fbe->context = NULL;
        }
        if (fbe->code) {
                free(fbe->code);
                fbe->code = NULL;
        }
}

enum fbe_rc fbe_compile(struct fbe *fbe, const char *sourcecode)
{
        if (sourcecode == NULL || sourcecode[0] == '\0' || fbe == NULL) {
                return FBE_INVALID_ARGUMENTS;
        }
        const char *end = sourcecode + strlen(sourcecode);
        struct FSM fsm  = {
            .p          = sourcecode,
            .pe         = end,
            .eof        = end,
            .parser     = ParseAlloc(malloc),
        };
        struct ast_node *root = NULL;
        if (!lexer(&fsm, &root)) {
                return FBE_COMPILATION_ERROR;
        }

        fbe->context = fbe_context_new();
        fbe->code    = fbe_ast2code(fbe->context, root);

        if (fbe->context == NULL || fbe->code == NULL) {
                fbe_clean(fbe);
                return FBE_FAIL;
        }

        return FBE_OK;
}
