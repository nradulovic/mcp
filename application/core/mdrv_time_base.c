/*
 * mdrv_time_base.c
 *
 *  Created on: Mar 8, 2021
 *      Author: nenad
 */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_tim.h"
#include "config_mdrv_time_base.h"
#include "mdrv.h"

static struct mdrv__context *g__mdrv_context;

void mdrv__time_base__init(struct mdrv__context *mdrv_context)
{
    g__mdrv_context = mdrv_context;
    MDRV_TIME_BASE_CONFIG__TIM_CLK_ENABLE();
    /* MDRV_TIME_BASE_CONFIG__TIM interrupt Init */
    HAL_NVIC_SetPriority(MDRV_TIME_BASE_CONFIG__TIM_IRQN, 0, 0);
    HAL_NVIC_EnableIRQ(MDRV_TIME_BASE_CONFIG__TIM_IRQN);
    MDRV_TIME_BASE_CONFIG__TIM->CR1 = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->DIER = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->SR = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->EGR = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->CCMR1 = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->CCER = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->CNT = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->PSC = (SystemCoreClock / 1) / 1000000u - 1u;
    MDRV_TIME_BASE_CONFIG__TIM->ARR = 9u;
    MDRV_TIME_BASE_CONFIG__TIM->CCR1 = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->OR = 0u;
    LL_TIM_GenerateEvent_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
}

#define COMPENSATION_START 1u

void mdrv__time_base__start(void *context, uint32_t period)
{
    (void) context;

    LL_TIM_SetAutoReload(MDRV_TIME_BASE_CONFIG__TIM, period - 1u);
    LL_TIM_GenerateEvent_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
    LL_TIM_EnableIT_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
    LL_TIM_SetCounter(MDRV_TIME_BASE_CONFIG__TIM, COMPENSATION_START);
    LL_TIM_EnableCounter(MDRV_TIME_BASE_CONFIG__TIM);
}
#include "main.h"

void mdrv__time_base__start_on_trigger(void *context, uint32_t period)
{
    (void) context;
    TIM_HandleTypeDef TimHandle;
    TimHandle.Instance = MDRV_TIME_BASE_CONFIG__TIM;
    TimHandle.Init.Period = period - 1;
    TimHandle.Init.Prescaler = (SystemCoreClock / 1) / 1000000u - 1u;
    TimHandle.Init.ClockDivision = 0;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Stop_IT(&TimHandle) != HAL_OK) {
        while (true)
            ;
    }
    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
        /* Initialization Error */
        while (true)
            ;
    }
//    LL_TIM_DisableCounter(MDRV_TIME_BASE_CONFIG__TIM);
//    LL_TIM_SetAutoReload(MDRV_TIME_BASE_CONFIG__TIM, period - 1u);
//    LL_TIM_GenerateEvent_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
//    LL_TIM_SetTriggerInput(MDRV_TIME_BASE_CONFIG__TIM, LL_TIM_TS_TI1FP1);
//    LL_TIM_SetSlaveMode(MDRV_TIME_BASE_CONFIG__TIM, LL_TIM_SLAVEMODE_TRIGGER);
//    LL_TIM_CC_DisableChannel(MDRV_TIME_BASE_CONFIG__TIM, LL_TIM_CHANNEL_CH1);
//    LL_TIM_IC_SetFilter(MDRV_TIME_BASE_CONFIG__TIM, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
//    LL_TIM_IC_SetPolarity(MDRV_TIME_BASE_CONFIG__TIM,
//                          LL_TIM_CHANNEL_CH1,
//                          LL_TIM_IC_POLARITY_FALLING);
//    /* Enable interrupt and timer */
//    LL_TIM_EnableIT_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
//    LL_TIM_EnableCounter(MDRV_TIME_BASE_CONFIG__TIM);

    TIM_SlaveConfigTypeDef sSlave = {
        .SlaveMode = TIM_SLAVEMODE_TRIGGER,
        .InputTrigger = TIM_TS_TI1FP1,
        .TriggerPolarity = TIM_TRIGGERPOLARITY_FALLING,
        .TriggerPrescaler = TIM_ETRPRESCALER_DIV1,
        .TriggerFilter = 0};

    if (HAL_TIM_SlaveConfigSynchro(&TimHandle, &sSlave) != HAL_OK) {
        while (true)
            ;
    }
    if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK) {
        while (true)
            ;
    }
}

void mdrv__time_base__stop(void *context)
{
    (void) context;
    LL_TIM_SetSlaveMode(MDRV_TIME_BASE_CONFIG__TIM, LL_TIM_SLAVEMODE_DISABLED);
    LL_TIM_DisableCounter(MDRV_TIME_BASE_CONFIG__TIM);
    LL_TIM_DisableIT_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
    LL_TIM_GenerateEvent_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
    LL_TIM_ClearFlag_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
}

void mdrv__time_base__it(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(MDRV_TIME_BASE_CONFIG__TIM)
        && LL_TIM_IsEnabledIT_UPDATE(MDRV_TIME_BASE_CONFIG__TIM)) {
        mdrv__it(g__mdrv_context);
        LL_TIM_ClearFlag_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
    }
}
