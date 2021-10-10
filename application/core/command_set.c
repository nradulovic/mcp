
/*
 * Copyright (C) IngBiro - Jovan Jovanovic
 * 
 * +381 63 477858 Jovan Jovanovic (info@ingbiro.net)
 * Vizantijski Bulevar 98/12
 */
/*
 * command_help.c
 *
 *  Created on: Mar 1, 2021
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
            NK_STRING__LITERAL("\r\nSet a value in MCP configuration (set) command syntax:\r\n"
                               "\r\nset <key> <value>\r\n"
                               "\r\n"
                               "  key   - Key name which needs to be set.\r\n"
                               "  value - A value which will be associated with the key.\r\n"));
    return NULL;
}
