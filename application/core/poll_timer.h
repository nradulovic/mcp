/*
 * timer.h
 *
 *  Created on: Aug 19, 2021
 *      Author: nenad
 */

#ifndef CORE_POLL_TIMER_H_
#define CORE_POLL_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

void poll_timer__init(void);
void poll_timer__setup_ms(uint32_t time_ms);
bool poll_timer__is_done(void);
void poll_timer__cleanup(void);
void poll_timer__isr(void);

#endif /* CORE_POLL_TIMER_H_ */
