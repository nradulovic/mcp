/*
 * command_help.h
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#ifndef COMMAND_HELP_H_
#define COMMAND_HELP_H_

#include "terminal/terminal.h"

const char* command_help__fn(void *terminal_context,
                             void *command_context,
                             size_t arg_count,
                             const char *arg_value[]);

#endif /* COMMAND_HELP_H_ */
