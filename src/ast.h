
#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Types of tokens in the AST
 */
enum tokens {
        TOKEN_AND, // operator logical and
        TOKEN_OR,  // operator logical or

        TOKEN_VARIABLE, // variable with name

        TOKEN_OPERATORS_BEGIN = TOKEN_AND,
        TOKEN_OPERATORS_END   = TOKEN_OR,

        TOKEN_FIRST = TOKEN_AND,
        TOKEN_LAST  = TOKEN_VARIABLE,
};

struct ast_node {
        enum tokens opcode;
        char *token;
        struct ast_node *left;
        struct ast_node *right;

        void *data; /* some additional data */
};

bool ast_node_operator(struct ast_node *node);

struct ast_node *ast_node_new(const int opcode);

struct ast_node *ast_node_variable(char *start, char *end);

struct ast_node *ast_node_root(const int opcode, struct ast_node *left,
                               struct ast_node *right);

void ast_node_graph(FILE *file, struct ast_node *root);

void *ParseAlloc(void *(*allocProc)(size_t));

void ParseFree(void *, void (*freeProc)(void *));

void Parse(void *parser,           /* The parser */
           int token_number,       /* The major token code number */
           struct ast_node *token, /* The value for the token */
           struct ast_node **root  /* Optional %extra_argument parameter */
           );

#endif /* !AST_H */
