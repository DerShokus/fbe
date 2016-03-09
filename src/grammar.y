
%nonassoc P_O  P_C.
%nonassoc VARIABLE.
/**
 * By priority to higher
 */
%left OR.
%left AND.
%nonassoc END.

%include {
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../src/ast.h"
}
%syntax_error { printf("Syntax error!\n"); }

/*
 * The %default_type directive specifies the datatype of non-terminal symbols
 * that do no have their own datatype defined using a separate %type directive.
 * See the documentation on %type below for addition information. */
%default_type { struct ast_node * }
%token_type { struct ast_node* }
%type OR  { enum tokens }
%type AND { enum tokens }

/**
 * The %extra_argument directive instructs Lemon to add a 4th parameter to
 * the parameter list of the Parse() function it generates. Lemon doesn't do
 * anything itself with this extra argument, but it does make the argument
 * available to C-code action routines, destructors, and so forth.
 */
%extra_argument { struct ast_node **ast_current }


program ::= operation(P) END.
{
    //printf("program ::= operation.\n");
    *ast_current = P;
}

/*
operation(F) ::= source_element(L) AND(O) source_element(R).
{
    //printf("operation ::= source_element AND source_element.\n");
    F = ast_node_root(O, L, R);
}
*/

operation(F) ::= operation(L) AND(O) operation(R).
{
    //printf("operation ::= operation AND operation.\n");
    F = ast_node_root(O, L, R);
}


/*
operation(F) ::= source_element(L) OR(O) source_element(R).
{
    //printf("operation ::= source_element OR source_element.\n");
    F = ast_node_root(O, L, R);
}
*/

operation(F) ::= operation(L) OR(O) operation(R).
{
    //printf("operation ::= operation OR operation.\n");
    F = ast_node_root(O, L, R);
}

operation(F) ::= P_O operation(O) P_C.
{
    F = O;
}


operation(B) ::= VARIABLE(A).
{
    //printf("operation ::= VARIABLE(%s).\n", A->token);
    B = A;
}

operation(B) ::= P_O VARIABLE(A) P_C.
{
    B = A;
}
