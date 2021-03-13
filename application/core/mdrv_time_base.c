/*
 * mdrv_time_base.c
 *
 *  Created on: Mar 8, 2021
 *      Author: nenad
 */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_tim.h"
#include "mdrv.h"

static struct mdrv__context *g__mdrv_context;

void mdrv__time_base__init(struct mdrv__context *mdrv_context)
{
    g__mdrv_context = mdrv_context;
    __HAL_RCC_TIM5_CLK_ENABLE();
    /* TIM5 interrupt Init */
    HAL_NVIC_SetPriority(TIM5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM5_IRQn);
    TIM5->CR1 = 0u;
    TIM5->DIER = 0u;
    TIM5->SR = 0u;
    TIM5->EGR = 0u;
    TIM5->CCMR1 = 0u;
    TIM5->CCER = 0u;
    TIM5->CNT = 0u;
    TIM5->PSC = (SystemCoreClock / 2) / 1000000u - 1u;
    TIM5->ARR = 9u;
    TIM5->CCR1 = 0u;
    TIM5->OR = 0u;
    LL_TIM_GenerateEvent_UPDATE(TIM5);
}

void mdrv__time_base__start(void *context, uint32_t period)
{
    (void) context;

    LL_TIM_DisableCounter(TIM5);
    LL_TIM_SetAutoReload(TIM5, period - 1u);
    LL_TIM_GenerateEvent_UPDATE(TIM5);
    /* Enable interrupt and timer */
    LL_TIM_EnableIT_UPDATE(TIM5);
    LL_TIM_EnableCounter(TIM5);
}

void mdrv__time_base__stop(void *context)
{
    (void) context;
    LL_TIM_DisableCounter(TIM5);
    LL_TIM_DisableIT_UPDATE(TIM5);
    LL_TIM_GenerateEvent_UPDATE(TIM5);
}

void mdrv__time_base__it(void)
{
    mdrv__it(g__mdrv_context);
}
