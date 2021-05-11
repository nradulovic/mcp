/*
 * command_help.c
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#include "generic/nk_string.h"

#include "command_get.h"

const char* command_get__fn(void *terminal_context,
                            void *command_context,
                            struct terminal_arguments *args,
                            struct nk_string *output)
{
    (void) terminal_context;
    (void) command_context;
    (void) args;

    nk_string__append_literal(
            output,
            NK_STRING__LITERAL("\n\rGet a value from MCP configuration (get) command syntax:\n\r"
                               "\n\rget <key>\n\r"
                               "\n\r"
                               "  key - Key name which needs to be get.\n\r"));
    return NULL;
}
