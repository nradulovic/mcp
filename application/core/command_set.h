/*
 * command_help.h
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#ifndef COMMAND_SET_H_
#define COMMAND_SET_H_

#include "terminal/terminal.h"

const char* command_set__fn(void *terminal_context,
                            void *command_context,
                            size_t arg_count,
                            const char *arg_value[]);

#endif /* COMMAND_SET_H_ */
