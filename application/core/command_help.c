/*
 * command_help.c
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#include "command_help.h"
#include "generic/nk_string.h"
#include "config_application.h"

const char* command_help__fn(void *terminal_context,
                             void *command_context,
                             struct terminal_arguments *args,
                             struct nk_string *output)
{
    (void) terminal_context;
    (void) command_context;
    (void) args;

    nk_string__append_literal(output,
                              NK_STRING__LITERAL(
                                  "\r\n## Manchester Code Proxy (MCP) "
                                  APP_VERSION_STRING
                                  " ("
                                  APP_TIMESTAMP_STRING
                                  ") command list ##\r\n"
                                  "\r\n"
                                  "help   - Prints the list of available commands\r\n"
                                  "set    - Set a value in MCP configuration\r\n"
                                  "get    - Get a value from MCP configuration\r\n"
                                  "rxchg  - Raw Exchange Data command\r\n")
                              );

    return NULL;
}
