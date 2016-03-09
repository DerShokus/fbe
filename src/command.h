
#ifndef BOOL_BYTE_CODE_H
#define BOOL_BYTE_CODE_H

#include <stdint.h>
#include <stddef.h>

#include "opcode.h"

/*
 * Structure of boolean byte code command
 *
 * +--------+--------+--------+----------------+~~~~~+----------------+
 * |        |        |        |+------+-------+|     |                |
 * | Opcode |Result's| Args   || Arg  | Arg   || ... |  Argument N    |
 * |        |Register| Count  || Type | Index || ... |                |
 * |        |   ID   |        |+------+-------+|     |                |
 * +--------+--------+--------+----------------+~~~~~+----------------+
 *     1B       1B       1B      2B or greater          2B or greater
 *
 */

#ifndef FBE_ARGUMENT_INDEX_TYPE
#define FBE_ARGUMENT_INDEX_TYPE uint8_t
#endif

/*
 * Argument of command (the last filed in the command)
 */
struct argument {
        uint8_t type; /* enum argument_type */
        union {
                uint8_t register_id;
                FBE_ARGUMENT_INDEX_TYPE external_id;
        };
};

enum argument_type {
        ARGUMENT_TYPE_REGISTER,
        ARGUMENT_TYPE_DEFAULT,
};

struct argument *argument_next(struct argument *argument);

/*
 * Command
 */
struct command {
        opcode_t opcode;
        uint8_t register_id;
        uint8_t args_count;
        struct argument args[1];
};

/*
 * TODO: DELETE IT
 */
uint8_t register_size(struct command *cmd);

/*
 * Like sizeof operator for the command
 * returns size in bytes
 */
size_t command_size(struct command *cmd);

void command_init(struct command *cmd);

struct command *command_next(struct command *cmd);

void command_printf(struct command *cmd);

/*
 * Command's arguments
 */
uint8_t command_args_count(struct command *cmd);

struct argument *command_arg_at(struct command *cmd, uint8_t index);

void command_arg_add(struct command *cmd, struct argument argument);

/*
 * Command's operation
 */
enum opcode command_opcode_get(struct command *cmd);

void command_opcode_set(struct command *cmd, enum opcode op);

/*
 * Result's register ID
 */
void command_register_set(struct command *cmd, uint8_t id);

uint8_t command_register_get(struct command *cmd);

#endif /* !BOOL_BYTE_CODE_H */
