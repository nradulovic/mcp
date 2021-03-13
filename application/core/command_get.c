/*
 * command_help.c
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#include "command_get.h"

const char* command_get__fn(void *terminal_context,
                            void *command_context,
                            size_t arg_count,
                            const char *arg_value[])
{
    (void) terminal_context;
    (void) command_context;
    (void) arg_count;
    (void) arg_value;

    static const char *help = "\n\rGet a value from MCP configuration (get) command syntax:\n\r"
        "\n\rget <key>\n\r"
        "\n\r"
        "  key - Key name which needs to be get.\n\r";
    return help;
}
