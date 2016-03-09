
#include <string.h>

#include "oparg.h"

opcode_arguments_t *opcode_argument_new(const size_t capasity)
{
        if (capasity < 1)
                return NULL;

        const size_t size =
            sizeof(opcode_arguments_t) + sizeof(OPARG_TYPE) * (capasity - 1);

        opcode_arguments_t *new = malloc(size);
        if (new != NULL) {
                memset(new, 0, size);
                new->count = capasity;
        }

        return new;
}

inline bool opcode_argument_get(const opcode_arguments_t *args,
                                const size_t position)
{
        if (args == NULL || position >= args->count)
                return false;

        return args->data[position] == 1;
}

void opcode_argument_set(opcode_arguments_t *args, const size_t position,
                         const bool value)
{
        if (args == NULL || args->count <= position)
                return;

        args->data[position] = value;
}
