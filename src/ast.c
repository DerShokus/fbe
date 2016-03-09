
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ast.h"

struct ast_node *ast_node_new(const int opcode)
{
        struct ast_node *node = malloc(sizeof(struct ast_node));
        memset(node, 0, sizeof(*node));
        node->opcode = opcode;

        return node;
}

struct ast_node *ast_node_variable(char *start, char *end)
{
        struct ast_node *node = ast_node_new(TOKEN_VARIABLE);

        const size_t size = end - start;
        char *token = malloc(size + 1);
        strncpy(token, start, size);
        token[size] = '\0';
        node->token = token;

        return node;
}

struct ast_node *ast_node_root(const int opcode, struct ast_node *left,
                               struct ast_node *right)
{
        struct ast_node *node = ast_node_new(opcode);
        node->right           = right;
        node->left            = left;

        return node;
}

static void ast_node_graph_name(struct ast_node *node, char *buffer,
                                size_t buffer_length)
{
        if (!node || !buffer || !buffer_length) {
                if (buffer && buffer_length)
                        buffer[0] = '\0';
                return;
        }
        int count = -1;
        if (node->opcode == TOKEN_VARIABLE) {
                count = snprintf(buffer, buffer_length, "VARIABLE(%s)",
                                 node->token ? node->token : "?");
        } else if (node->opcode == TOKEN_OR) {
                count = snprintf(buffer, buffer_length, "OR");
        } else if (node->opcode == TOKEN_AND) {
                count = snprintf(buffer, buffer_length, "AND");
        }
        if (count == -1)
                buffer[0] = '\0';
}

static void ast_node_graph_print(FILE *file, struct ast_node *node)
{
        if (node) {
                /* set label for the node */
                char name[256] = {0}, buff[512] = {0};
                ast_node_graph_name(node, name, sizeof(name));
                snprintf(buff, sizeof(buff), "%u [label=\"%s\"];\n",
                         (unsigned)node, name);
                fwrite(buff, 1, strlen(buff), file);
                /* set children for the node */
                if (node->left) {
                        snprintf(buff, sizeof(buff),
                                 "%u -- %u [label=\"left\"];\n", (unsigned)node,
                                 (unsigned)node->left);
                        fwrite(buff, 1, strlen(buff), file);
                }
                if (node->right) {
                        snprintf(buff, sizeof(buff),
                                 "%u -- %u [label=\"right\"];\n",
                                 (unsigned)node, (unsigned)node->right);
                        fwrite(buff, 1, strlen(buff), file);
                }
        }
}

void ast_node_graph(FILE *file, struct ast_node *root)
{
        if (!root)
                return;

        char start[]                = "graph ast {\n";
        char end[]                  = "}\n\n";
        struct ast_node *pick[1024] = {0};
        int top                     = -1;

        struct ast_node *node = root;

        fwrite(start, 1, strlen(start), file);

        pick[++top] = root;
        while (top >= 0) {
                node = pick[top--];
                ast_node_graph_print(file, node);
                if (node->left)
                        pick[++top] = node->left;
                if (node->right)
                        pick[++top] = node->right;
        }

        fwrite(end, 1, strlen(end), file);
}

bool ast_node_operator(struct ast_node *node)
{
        if (node->opcode > TOKEN_OPERATORS_END ||
            node->opcode < TOKEN_OPERATORS_BEGIN)
                return false;
        return true;
}
