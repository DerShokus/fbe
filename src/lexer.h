
#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include "../src/ast.h"

/*
 * Struct of lexer fsm
 */
struct FSM {
        const char *p;   // data pointer
        const char *pe;  // data end pointer
        const char *eof; // end of file pointer
        int cs;          // TODO: write a comment
        int *stack;      // store integer values representing states
        int top;         // offset of stack
        int act;         // TODO: write a comment
        const char *ts;  // template start pointer
        const char *te;  // template end pointer
        void *parser;    // grammar parser (look at grammar.(y|h|c))
};

/**
 * Process a string
 */
bool lexer(struct FSM *fsm, struct ast_node **root);
#endif /* !LEXER_H */
