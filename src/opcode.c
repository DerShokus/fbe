
#include <assert.h>

#include "opcode.h"

enum opcode opcode_get(opcode_t *opcode)
{
        assert(*opcode >= OPCODE_FIRST && *opcode <= OPCODE_LAST);

        return (enum opcode)(*opcode);
}

void opcode_set(opcode_t *opcode, enum opcode code)
{
        assert(*opcode >= OPCODE_FIRST && *opcode <= OPCODE_LAST);
        *opcode = (opcode_t)code;
}
