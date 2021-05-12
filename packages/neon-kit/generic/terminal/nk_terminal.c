/*
 * nk_terminal.c
 *
 *  Created on: Apr 29, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 */

#include <stdint.h>

#include "generic/nk_string.h"
#include "generic/terminal/nk_terminal.h"

static int
process_command_line(struct terminal_descriptor *terminal,
                     const struct nk_string *command_string,
                     struct nk_string *output)
{
    struct
        NK_STRING__BUCKET_T(1)
    delimiter = NK_STRING__BUCKET_INITIALIZER(&delimiter, " ");
    struct nk_string__find__result delimiter_n;
    struct nk_string command_view;
    struct nk_string token_view;
    struct
        NK_ARRAY__BUCKET_TYPED_T(struct nk_string, 16u, struct terminal_arguments)
    tokens = NK_ARRAY__BUCKET_INITIALIZER_EMPTY(&tokens);

    /* If we have just enter (\r or \n) just return */
    if (command_string->length <= 1u) {
        return 0;
    }
    /* Remove last character which is either \r or \n and create token views */
    command_view = nk_string__view(command_string, 0, command_string->length - 1u);

    for (size_t token_i = 0u; token_i < tokens.array.item_no; token_i++) {
        delimiter_n = nk_string__find(&command_view, &delimiter.array, 0, SIZE_MAX);
        if (delimiter_n.error) {
            tokens.array.items[token_i] = command_view;
            tokens.array.length++;
            break;
        } else {
            token_view = nk_string__view(&command_view, 0, delimiter_n.value);
            tokens.array.items[token_i] = token_view;
            tokens.array.length++;
        }
        command_view = nk_string__view(&command_view, delimiter_n.value + 1, SIZE_MAX);
    }
    /* First token is command, find the command */
    for (size_t command_i = 0u; command_i < terminal->p__commands->length; command_i++) {
        const struct terminal__command_descriptor *command = terminal->p__commands->items[command_i];

        if (nk_string__is_equal(&tokens.array.items[0], command->command_id)) {
            command->interpreter.fn(terminal->p__terminal_context,
                                    command->interpreter.command_context,
                                    &tokens.array,
                                    output);
            return 0;
        }
    }
    return 1;
}

void
terminal__init(struct terminal_descriptor *terminal,
               const struct terminal_commands *commands,
               struct nk_string *arg_buffer,
               struct nk_string *error_message)
{
    terminal->p__commands = commands;
    terminal->p__working_buffer = arg_buffer;
    terminal->p__error_message = error_message;
}

void
terminal__interpret(struct terminal_descriptor *terminal, const struct nk_string *input, struct nk_string *output)
{
    struct nk_string__find__result find_n;
    struct
        NK_STRING__BUCKET_T(1)
    enter_r = NK_STRING__BUCKET_INITIALIZER(&enter_r, "\r");
    struct
        NK_STRING__BUCKET_T(1)
    enter_n = NK_STRING__BUCKET_INITIALIZER(&enter_n, "\n");
    struct
        NK_STRING__BUCKET_T(2)
    enter_nn = NK_STRING__BUCKET_INITIALIZER(&enter_n, "\n\n");
    struct
        NK_STRING__BUCKET_T(2)
    double_space = NK_STRING__BUCKET_INITIALIZER(&double_space, "  ");
    struct
        NK_STRING__BUCKET_T(2)
    single_space = NK_STRING__BUCKET_INITIALIZER(&single_space, " ");

    nk_string__copy(output, input);
    nk_string__append(terminal->p__working_buffer, input);
    nk_string__replace(terminal->p__working_buffer, &enter_r.array, &enter_n.array);
    nk_string__replace(terminal->p__working_buffer, &enter_nn.array, &enter_n.array);
    while (nk_string__contains(terminal->p__working_buffer, &double_space.array)) {
        nk_string__replace(terminal->p__working_buffer, &double_space.array, &single_space.array);
    }

    do {
        struct nk_string command_string;

        /* See if the user pressed enter */
        find_n = nk_string__find(terminal->p__working_buffer, &enter_n.array, 0, SIZE_MAX);

        if (find_n.error != NK_ERROR__OK) {
            break;
        }

        /* Found '\n' in the string, send the string to process and then remove it from working buffer */
        command_string = nk_string__view(terminal->p__working_buffer, 0u, find_n.value + 1u);
        if (process_command_line(terminal, &command_string, output) == 1) {
            if (terminal->p__error_message) {
                nk_string__append(output, terminal->p__error_message);
            } else {
                nk_string__append_literal(output, NK_STRING__LITERAL("unknown command\r\n"));
            }
        }
        nk_string__lstrip(terminal->p__working_buffer, &command_string);
    } while (true);
}

void
terminal__discard_command(struct terminal_descriptor *terminal)
{
    nk_string__clear_all(terminal->p__working_buffer);
}

void
terminal__set_terminal_context(struct terminal_descriptor *terminal, void *terminal_context)
{
    terminal->p__terminal_context = terminal_context;
}
