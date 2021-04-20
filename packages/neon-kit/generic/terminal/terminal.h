/*
 * terminal.h
 *
 *  Created on: Apr 13, 2021
 *      Author: nenad
 */

#ifndef GENERIC_TERMINAL_TERMINAL_H_
#define GENERIC_TERMINAL_TERMINAL_H_

#include <stddef.h>

typedef const char* (command_interpreter_fn)(void *p__terminal_context,
                                             void *command_context,
                                             size_t arg_count,
                                             const char *arg_value[]);

struct terminal__command_interpreter
{
    command_interpreter_fn *fn;
    void *command_context;
};

struct terminal__command_descriptor
{
    const char *command_id;
    struct terminal__command_interpreter interpreter;
};

struct terminal_descriptor
{
    const struct terminal__command_descriptor *p__commands;
    size_t p__no_commands;
    char *p__arg_buffer;
    size_t p__arg_size;
    void *p__terminal_context;
    size_t p__content_index;
    const char *error_message;
};

void terminal__init(struct terminal_descriptor *terminal,
                    const struct terminal__command_descriptor *commands,
                    size_t commands_size,
                    char *command_arg_buffer,
                    size_t command_arg_size);
const char* terminal__interpret(struct terminal_descriptor *terminal,
                                const char *input,
                                size_t input_length);
void terminal__discard_command(struct terminal_descriptor *terminal);
void terminal__set_terminal_context(struct terminal_descriptor *terminal, void *terminal_context);


#endif /* GENERIC_TERMINAL_TERMINAL_H_ */
