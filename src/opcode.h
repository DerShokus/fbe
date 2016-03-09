
#ifndef OPCODE_H
#define OPCODE_H

#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#include "../include/oparg.h"
#include "../src/ast.h"

typedef uint8_t opcode_t; /* opcode 4b */

/*
 *
 */
enum opcode {
        /* Logical expressions */
        OPCODE_LOGICAL_AND = 0x1,
        OPCODE_LOGICAL_OR  = 0x2,
        /* Stack operations */
        OPCODE_STACK_PUSH = 0x3,
        OPCODE_STACK_POP  = 0x4,
        /* Should be removed */
        OPCODE_END = 0xFF, // ending terminator (is it bad idea?)
        /* Limits */
        OPCODE_FIRST = OPCODE_LOGICAL_AND,
        OPCODE_LAST  = OPCODE_END,
};

enum opcode opcode_get(opcode_t *opcode);

void opcode_set(opcode_t *opcode, enum opcode);

#endif /* !BYTECODE_H */
