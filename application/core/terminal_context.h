/*
 * terminal_context.h
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#ifndef TERMINAL_CONTEXT_H_
#define TERMINAL_CONTEXT_H_

struct terminal_context
{
    uint8_t (transmit__fn)(const void * buffer, uint16_t length);
};

#endif /* TERMINAL_CONTEXT_H_ */
