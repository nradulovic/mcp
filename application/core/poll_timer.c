/*
 * timer.c
 *
 *  Created on: Aug 19, 2021
 *      Author: nenad
 */

#include "poll_timer.h"
#include "stm32f4xx.h"
#include "config_peripherals.h"

static uint32_t g__delay_counter;
static uint32_t g__delay_period;

void poll_timer__init(void)
{
    HAL_NVIC_DisableIRQ(SysTick_IRQn);
    /*
     * Set reload register to generate an interrupt every millisecond.
     */
    SysTick->LOAD = (uint32_t)((SystemCoreClock / 1000u) - 1u);

    /*
     * Reset the SysTick counter value.
     */
    SysTick->VAL = 0ul;

    /*
     * Set SysTick source and IRQ.
     */
    SysTick->CTRL = (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk);

    HAL_NVIC_SetPriority(SysTick_IRQn, POLL_TIMER__IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(SysTick_IRQn);
}

void poll_timer__setup_ms(uint32_t time_ms)
{
    g__delay_period = time_ms;
    g__delay_counter = 0ul;
    /*
     * Enable the SysTick timer
     */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

bool poll_timer__is_done(void)
{
    if (g__delay_counter < g__delay_period) {
        return false;
    } else {
        return true;
    }
}

void poll_timer__cleanup(void)
{
    /*
     * Disable the SysTick timer
     */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void poll_timer__isr(void)
{
    g__delay_counter++;
}
