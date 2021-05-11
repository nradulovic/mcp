/*
 * command_help.h
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#ifndef COMMAND_GET_H_
#define COMMAND_GET_H_

#include "generic/terminal/nk_terminal.h"

const char* command_get__fn(void *terminal_context,
                            void *command_context,
                            struct terminal_arguments *args,
                            struct nk_string *output);

#endif /* COMMAND_GET_H_ */
