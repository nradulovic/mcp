/*
 * command_help.c
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#include "command_help.h"

const char* command_help__fn(void *terminal_context,
                             void *command_context,
                             size_t arg_count,
                             const char *arg_value[])
{
    static const char *const help_text = "\r\n"
        "## Command list ##\r\n"
        "\r\n"
        "help  - Prints the list of available commands\r\n"
        "set   - Set a value in MCP configuration\r\n"
        "get   - Get a value from MCP configuration\r\n"
        "write - Write data to anyCS\r\n"
        "read  - Read data from anyCS\r\n"
        "xchg  - Exchange data with anyCS\r\n";

    (void) terminal_context;
    (void) command_context;
    (void) arg_count;
    (void) arg_value;

    return help_text;
}
