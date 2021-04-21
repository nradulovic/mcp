/*
 * command_rxchg.h
 *
 *  Created on: Mar 10, 2021
 *      Author: nenad
 */

#ifndef COMMAND_RXCHG_H_
#define COMMAND_RXCHG_H_

#include "terminal/terminal.h"

struct nk_string;

const char* command_rxchg__fn(void *terminal_context,
                              void *command_context,
                              size_t arg_count,
                              const char *arg_value[]);

#endif /* COMMAND_RXCHG_H_ */
