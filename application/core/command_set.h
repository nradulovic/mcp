
/*
 * Copyright (C) IngBiro - Jovan Jovanovic
 * 
 * +381 63 477858 Jovan Jovanovic (info@ingbiro.net)
 * Vizantijski Bulevar 98/12
 */
/*
 * command_set.h
 *
 *  Created on: Mar 1, 2021
 */

#ifndef COMMAND_SET_H_
#define COMMAND_SET_H_

#include "generic/terminal/nk_terminal.h"

const char* command_set__fn(void *terminal_context,
                            void *command_context,
                            struct terminal_arguments *args,
                            struct nk_string *output);

#endif /* COMMAND_SET_H_ */
