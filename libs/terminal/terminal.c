/*
 * terminal.c
 *
 *  Created on: Feb 28, 2021
 *      Author: nenad
 */

#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "terminal.h"

static bool str__equal(const char *string_left, const char *string_right)
{
    if (strcmp(string_left, string_right) == 0) {
        return true;
    } else {
        return false;
    }
}

static size_t str__ncopy(const char *from, size_t from_length, char *to, size_t to_buffer_size)
{
    size_t to_copy_length = from_length;

    if (to_copy_length >= to_buffer_size) {
        to_copy_length = to_buffer_size - 1;
    }
    strncpy(to, from, to_copy_length);
    to[to_copy_length] = '\0';
    return from_length - to_copy_length;
}

static bool str__find_character(const char *string, char character, size_t *cursor_index)
{
    for (size_t i = 0u; string[i] != '\0'; i++) {
        if (string[i] == character) {
            *cursor_index = i;
            return true;
        }
    }
    return false;
}

static int copy_to_buffer(struct terminal_descriptor *terminal,
                          const char *input,
                          size_t input_length)
{
    size_t copy_left;
    size_t free_space = terminal->command_arg_size - terminal->p__content_index;

    copy_left = str__ncopy(input,
                           input_length,
                           &terminal->command_arg_buffer[terminal->p__content_index],
                           free_space);
    if (copy_left != 0) {
        return -1;
    }
    terminal->p__content_index += input_length;
    return 0;
}

static int command_is_complete(const struct terminal_descriptor *terminal, size_t *cursor_index)
{
    bool status;

    status = str__find_character(terminal->command_arg_buffer, '\r', cursor_index);

    if (status == true) {
        (*cursor_index)++;
        return true;
    }

    status = str__find_character(terminal->command_arg_buffer, '\n', cursor_index);

    if (status == true) {
        (*cursor_index)++;
    }
    return status;
}

static int tokenize(const struct terminal_descriptor *terminal, size_t cursor_index)
{
    for (size_t i = 0; i < cursor_index; i++) {
        char *current = &terminal->command_arg_buffer[i];

        if (*current == ' ') {
            *current = '\0';
        }
        if (*current == '\r') {
            *current = '\0';
        }
        if (*current == '\n') {
            *current = '\0';
        }
    }
    return 0;
}

static int parse(const struct terminal_descriptor *terminal,
                 const char **arg_value,
                 size_t arg_value_entries,
                 size_t *arg_count,
                 size_t cursor_index)
{
    enum sm_state
    {
        STATE_NULL,
        STATE_FOUND_TOKEN,
    } sm_state = STATE_NULL;

    *arg_count = 0;

    for (size_t i = 0; i < cursor_index; i++) {
        const char *character = &terminal->command_arg_buffer[i];

        switch (sm_state) {
        case STATE_NULL:
            if (*character != '\0') {
                arg_value[(*arg_count)++] = character;

                if (*arg_count == arg_value_entries) {
                    return -1;
                }
                sm_state = STATE_FOUND_TOKEN;
            }
            break;
        case STATE_FOUND_TOKEN:
            if (*character == '\0') {
                sm_state = STATE_NULL;
            }
            break;
        default:
            return -1;
        }
    }
    return 0;
}

static const struct terminal__command_descriptor* find_command(const struct terminal_descriptor *terminal,
                                                               const char *command)
{
    for (size_t i = 0; i < terminal->no_commands; i++) {
        const char *command_id = terminal->commands[i].command_id;

        if (str__equal(command, command_id)) {
            return &terminal->commands[i];
        }
    }
    return NULL;
}

static const char* process_command(struct terminal_descriptor *terminal, size_t cursor_index)
{
    const struct terminal__command_descriptor *command;
    const char *arg_value[16];
    size_t arg_count;
    int error;

    error = tokenize(terminal, cursor_index);
    if (error) {
        return terminal->error_message;
    }
    error = parse(terminal, arg_value, 16, &arg_count, cursor_index);

    if (error) {
        return terminal->error_message;
    }
    command = find_command(terminal, arg_value[0]);

    if (command == NULL) {
        return terminal->error_message;
    }
    return command->interpreter.fn(terminal->terminal_context,
                                   command->interpreter.command_context,
                                   arg_count,
                                   arg_value);
}

const char* terminal__interpret(struct terminal_descriptor *terminal,
                                const char *input,
                                size_t input_length)
{
    int error;
    size_t cursor_index;
    const char *response;

    error = copy_to_buffer(terminal, input, input_length);
    if (error) {
        return terminal->error_message;
    }
    if (command_is_complete(terminal, &cursor_index)) {
        response = process_command(terminal, cursor_index);
        memmove(terminal->command_arg_buffer,
                &terminal->command_arg_buffer[cursor_index],
                terminal->p__content_index - cursor_index);
        terminal->p__content_index -= cursor_index;
        memset(&terminal->command_arg_buffer[cursor_index],
               0,
               terminal->command_arg_size - terminal->p__content_index);
    } else {
        response = &terminal->command_arg_buffer[terminal->p__content_index - 1u];
    }
    return response;
}

void terminal__discard_command(struct terminal_descriptor *terminal)
{
    terminal->p__content_index = 0;
    memset(&terminal->command_arg_buffer, 0, terminal->command_arg_size);
}
