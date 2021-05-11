/*
 * command_rxchg.h
 *
 *  Created on: Mar 10, 2021
 *      Author: nenad
 */

#ifndef COMMAND_RXCHG_H_
#define COMMAND_RXCHG_H_

#include "generic/terminal/nk_terminal.h"

const char* command_rxchg__fn(void *terminal_context,
                            void *command_context,
                            struct terminal_arguments *args,
                            struct nk_string *output);

#endif /* COMMAND_RXCHG_H_ */
