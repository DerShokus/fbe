
#ifndef OPARG_H
#define OPARG_H

#include <stdlib.h>
#include <stdbool.h>

#ifndef OPARG_TYPE
#define OPARG_TYPE int8_t
#endif

typedef struct opcode_arguments {
        size_t count;
        // for storing data with all structure
        // (actually this is a minimal count of data)
        OPARG_TYPE data[1];

} opcode_arguments_t;

opcode_arguments_t *opcode_argument_new(const size_t capasity);

extern bool opcode_argument_get(const opcode_arguments_t *args,
                                const size_t position);

// TODO: make retcodes enum
void opcode_argument_set(opcode_arguments_t *args, const size_t position,
                         const bool value);

#endif /* !OPARG_H */
