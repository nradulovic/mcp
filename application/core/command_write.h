/*
 * command_help.h
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#ifndef COMMAND_WRITE_H_
#define COMMAND_WRITE_H_

#include "terminal.h"

const char* command_write__fn(void *terminal_context,
                              void *command_context,
                              size_t arg_count,
                              const char *arg_value[]);

#endif /* COMMAND_WRITE_H_ */
