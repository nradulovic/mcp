
/*
 * Copyright (C) IngBiro - Jovan Jovanovic
 * 
 * +381 63 477858 Jovan Jovanovic (info@ingbiro.net)
 * Vizantijski Bulevar 98/12
 */
/*
 * command_rxchg.h
 *
 *  Created on: Mar 10, 2021
 */

#ifndef COMMAND_RXCHG_H_
#define COMMAND_RXCHG_H_

#include "generic/terminal/nk_terminal.h"

const char* command_rxchg__fn(void *terminal_context,
                            void *command_context,
                            struct terminal_arguments *args,
                            struct nk_string *output);

#endif /* COMMAND_RXCHG_H_ */
