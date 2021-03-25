/*
 * command_help.c
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#include "command_set.h"

const char* command_set__fn(void *terminal_context,
                            void *command_context,
                            size_t arg_count,
                            const char *arg_value[])
{
    (void) terminal_context;
    (void) command_context;
    (void) arg_count;
    (void) arg_value;

    static const char *help = "\n\rSet a value in MCP configuration (set) command syntax:\n\r"
        "\n\rset <key> <value>\n\r"
        "\n\r"
        "  key   - Key name which needs to be set.\n\r"
        "  value - A value which will be associated with the key.\n\r";
    return help;
}
