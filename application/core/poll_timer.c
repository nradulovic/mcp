
/*
 * Copyright (C) IngBiro - Jovan Jovanovic
 * 
 * +381 63 477858 Jovan Jovanovic (info@ingbiro.net)
 * Vizantijski Bulevar 98/12
 */
/*
 * poll_timer.c
 *
 *  Created on: Aug 19, 2021
 */

#include "poll_timer.h"
#include "stm32f4xx.h"
#include "config_peripherals.h"

static uint32_t g__delay_counter;
static uint32_t g__delay_period;

void poll_timer__init(void)
{
    HAL_InitTick(POLL_TIMER__IRQ_PRIO);
}

void poll_timer__setup_ms(uint32_t time_ms)
{
    g__delay_period = time_ms;
    g__delay_counter = 0ul;
}

bool poll_timer__is_done(void)
{
    if (g__delay_period == 0u) {
        return true;
    }
    if (g__delay_counter < g__delay_period) {
        return false;
    } else {
        return true;
    }
}

void poll_timer__cleanup(void)
{
    g__delay_period = 0u;
}

void poll_timer__isr(void)
{
    g__delay_counter++;
}
