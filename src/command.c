
#include <assert.h>
#include <string.h>

#include "command.h"

size_t command_size(struct command *command)
{
        const size_t size_of_args =
            sizeof(struct argument) * command_args_count(command);

        return sizeof(opcode_t) + /* opcde */
               sizeof(uint8_t) +  /* register_id */
               sizeof(uint8_t) +  /* args_count */
               size_of_args;
}

void command_init(struct command *cmd)
{
        cmd->opcode      = OPCODE_END;
        cmd->register_id = UINT8_MAX;
        cmd->args_count  = 0;
}

struct command *command_next(struct command *cmd)
{
        const char *p = (char *)cmd;
        return (struct command *)(p + command_size(cmd));
}

void command_printf(struct command *cmd)
{
        printf("command {\n");
        printf("\topcode: %u,\tregister_id: %u,\targs_count: %u\n", cmd->opcode,
               cmd->register_id, cmd->args_count);
        for (uint8_t i = 0; i < command_args_count(cmd); ++i) {
                struct argument *arg = command_arg_at(cmd, i);
                printf("\targ[%u]: type: %s,\tindex: %u\n", i,
                       arg->type == ARGUMENT_TYPE_DEFAULT ? "DEFAULT"
                                                          : "REGISTER",
                       arg->type == ARGUMENT_TYPE_DEFAULT ? arg->external_id
                                                          : arg->register_id);
        }
        printf("}\n");
}

uint8_t command_args_count(struct command *command)
{
        return command->args_count;
}

struct argument *command_arg_at(struct command *cmd, uint8_t index)
{
        assert(index < command_args_count(cmd));
        return &cmd->args[index];
}

void command_arg_add(struct command *cmd, struct argument argument)
{
        uint8_t total   = command_args_count(cmd);
        cmd->args_count = total + 1;
        // TODO: проверка на пределы
        memcpy(&cmd->args[total], &argument, sizeof(argument));
}

enum opcode command_opcode_get(struct command *command)
{
        return opcode_get(&command->opcode);
}

void command_opcode_set(struct command *command, enum opcode code)
{
        opcode_set(&command->opcode, code);
}

void command_register_set(struct command *command, uint8_t id)
{
        command->register_id = id;
}

uint8_t command_register_get(struct command *command)
{
        return command->register_id;
}

// TODO: DELETE IT
uint8_t register_size(struct command *cmd)
{
        uint8_t res = 0;
        while (cmd->opcode != OPCODE_END) {
                res = cmd->register_id < res ? res : cmd->register_id;
                cmd = command_next(cmd);
        }
        return res;
}
