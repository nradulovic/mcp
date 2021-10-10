
/*
 * Copyright (C) IngBiro - Jovan Jovanovic
 * 
 * +381 63 477858 Jovan Jovanovic (info@ingbiro.net)
 * Vizantijski Bulevar 98/12
 */
/*
 * mdrv_time_base.h
 *
 *  Created on: Mar 8, 2021
 */

#ifndef MDRV_APPLICATION_H_
#define MDRV_APPLICATION_H_

#include <stdint.h>
#include <stdbool.h>

struct mdrv__context;

void mdrv__application__init(struct mdrv__context * mdrv_context);
void mdrv__application__tim__start(void *context, uint32_t period);
void mdrv__application__tim__start_on_trigger(void *context, uint32_t period);
void mdrv__application__tim__stop(void *context);
void mdrv__application__tim__isr(void);
void mdrv__application__pin__init_output(void *context, bool value);
void mdrv__application__pin__init_input(void *context);
void mdrv__application__pin__init_trigger(void *context);
void mdrv__application__pin__write(void *context, bool value);
bool mdrv__application__pin__read(void *context);

#endif /* MDRV_APPLICATION_H_ */
