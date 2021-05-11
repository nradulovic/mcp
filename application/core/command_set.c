/*
 * command_help.c
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#include "generic/nk_string.h"
#include "command_set.h"

const char* command_set__fn(void *terminal_context,
                            void *command_context,
                            struct terminal_arguments *args,
                            struct nk_string *output)
{
    (void) terminal_context;
    (void) command_context;
    (void) args;

    nk_string__append_literal(
            output,
            NK_STRING__LITERAL("\n\rSet a value in MCP configuration (set) command syntax:\n\r"
                               "\n\rset <key> <value>\n\r"
                               "\n\r"
                               "  key   - Key name which needs to be set.\n\r"
                               "  value - A value which will be associated with the key.\n\r"));
    return NULL;
}
