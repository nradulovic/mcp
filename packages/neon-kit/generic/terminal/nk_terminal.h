/*
 * nk_terminal.h
 *
 *  Created on: Apr 13, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 */

#ifndef GENERIC_TERMINAL_TERMINAL_H_
#define GENERIC_TERMINAL_TERMINAL_H_

#include <stddef.h>

#include "generic/nk_array.h"

#define NK_ENABLED_TERMINAL

struct nk_string;


struct terminal_arguments
NK_ARRAY__T(struct nk_string);

typedef const char*
(command_interpreter_fn)(void *terminal_context, void *command_context, struct terminal_arguments *args, struct nk_string * output);

struct terminal__command_interpreter
{
    command_interpreter_fn *fn;
    void *command_context;
};


struct terminal__command_descriptor
{
    struct nk_string *command_id;
    struct terminal__command_interpreter interpreter;
};

struct terminal_commands
NK_ARRAY__T(const struct terminal__command_descriptor *);


struct terminal_descriptor
{
    const struct terminal_commands *p__commands;
    struct nk_string *p__working_buffer;
    struct nk_string *p__error_message;
    void * p__terminal_context;
};

void
terminal__init(struct terminal_descriptor *terminal,
               const struct terminal_commands *commands,
               struct nk_string *working_buffer,
               struct nk_string *error_message);

void
terminal__interpret(struct terminal_descriptor *terminal, const struct nk_string *input, struct nk_string *output);
void
terminal__discard_command(struct terminal_descriptor *terminal);
void
terminal__set_terminal_context(struct terminal_descriptor *terminal, void *terminal_context);

#endif /* GENERIC_TERMINAL_TERMINAL_H_ */
