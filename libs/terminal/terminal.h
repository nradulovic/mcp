/*
 * terminal.h
 *
 *  Created on: Feb 28, 2021
 *      Author: nenad
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <stddef.h>

typedef const char* (command_interpreter_fn)(void *terminal_context,
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
    const struct terminal__command_descriptor *commands;
    size_t no_commands;
    const char *error_message;
    char *command_arg_buffer;
    size_t command_arg_size;
    void *terminal_context;
    size_t p__content_index;
};

const char* terminal__interpret(struct terminal_descriptor *terminal,
                                const char *input,
                                size_t input_length);
void terminal__discard_command(struct terminal_descriptor * terminal);

#endif /* TERMINAL_H_ */
