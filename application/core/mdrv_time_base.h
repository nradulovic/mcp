/*
 * mdrv_time_base.h
 *
 *  Created on: Mar 8, 2021
 *      Author: nenad
 */

#ifndef MDRV_TIME_BASE_H_
#define MDRV_TIME_BASE_H_

#include <stdint.h>

struct mdrv__context;

void mdrv__time_base__init(struct mdrv__context * mdrv_context);
void mdrv__time_base__start(void *context, uint32_t period);
void mdrv__time_base__start_on_trigger(void *context, uint32_t period);
void mdrv__time_base__stop(void *context);
void mdrv__time_base__it(void);

#endif /* MDRV_TIME_BASE_H_ */
