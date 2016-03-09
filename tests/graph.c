/*
 * graph.c
 * Copyright (C) 2016 DerShokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include <string.h>
#include "../src/lexer.h"

int main(int argc, char *argv[])
{
        if (argc != 2)
                return -1;

        const char *source = argv[1];
        const char *end    = source + strlen(source);
        struct FSM fsm     = {
            .p = source, .pe = end, .eof = end, .parser = ParseAlloc(malloc),
        };
        struct ast_node *root = NULL;
        if (!lexer(&fsm, &root)) {
                return -1;
        }
        FILE *file = fopen("out.dot", "w");
        ast_node_graph(file, root);
        fclose(file);

        return 0;
}
